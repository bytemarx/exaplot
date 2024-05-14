/*
 * ExaPlot
 * data management unit
 * 
 * SPDX-License-Identifier: GPL-3.0
 * Copyright (C) 2024 bytemarx
 */

#pragma once

#include "hdf5.h"

#include <cassert>
#include <cstdlib>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>


template<typename T>
class DataSet
{
public:
    DataSet(hid_t fileID, const std::string& name, hsize_t numElements, hid_t datatype)
    : m_valid{true}
    , m_datatype{datatype}
    {
        hsize_t chunkDim[] = {4096, numElements};
        auto propertyList = H5Pcreate(H5P_DATASET_CREATE);
        if (propertyList == H5I_INVALID_HID) {
            throw std::runtime_error{"error creating HDF5 property list"};
        }
        if (H5Pset_chunk(propertyList, 2, chunkDim) < 0) {
            H5Pclose(propertyList);
            throw std::runtime_error{"error setting chunk property"};
        }

        hsize_t dim[] = {0, numElements};
        hsize_t maxDim[] = {H5S_UNLIMITED, numElements};
        auto dataspace = H5Screate_simple(2, dim, maxDim);
        if (dataspace == H5I_INVALID_HID) {
            H5Pclose(propertyList);
            throw std::runtime_error{"error creating HDF5 dataspace"};
        }

        this->m_dataset = H5Dcreate(
            fileID,
            name.c_str(),
            datatype,
            dataspace,
            H5P_DEFAULT,
            propertyList,
            H5P_DEFAULT
        );
        assert(H5Sclose(dataspace) >= 0);
        assert(H5Pclose(propertyList) >= 0);
        if (this->m_dataset == H5I_INVALID_HID) {
            throw std::runtime_error{"error creating HDF5 dataset"};
        }
    }

    DataSet(const DataSet&) = delete;

    DataSet& operator=(const DataSet&) = delete;

    DataSet(DataSet&& other)
        : m_valid{other.m_valid}
        , m_buffer{std::move(other.m_buffer)}
        , m_dataset{other.m_dataset}
    {
        other.m_valid = false;
        other.m_dataset = H5I_INVALID_HID;
    }

    DataSet& operator=(DataSet&&) = delete;

    virtual ~DataSet()
    {
        if (this->m_valid) {
            this->flush();
            H5Dclose(this->m_dataset);
        }
    }

    int flush()
    {
        auto status = this->writeToDataset();
        if (status < 0)
            return status;
        return H5Dflush(this->m_dataset);
    }

protected:
    int writeToDataset()
    {
        int status = 0;

        if (this->m_buffer.size() == 0)
            return status;

        auto dataspaceID = H5Dget_space(this->m_dataset);
        if (dataspaceID == H5I_INVALID_HID)
            return -1;

        // retrieve current dataset dimensions
        hsize_t currentDims[2] = {0};
        status = H5Sget_simple_extent_dims(dataspaceID, currentDims, NULL);
        H5Sclose(dataspaceID);
        if (status < 0)
            return status;

        hsize_t length = static_cast<hsize_t>(this->m_buffer.size()) / currentDims[1];

        // extend the dataset
        hsize_t updatedDims[2] = {currentDims[0] + length, currentDims[1]};
        if (status = H5Dset_extent(this->m_dataset, updatedDims), status < 0)
            return status;

        // select the extended region
        auto filespaceID = H5Dget_space(this->m_dataset);
        if (filespaceID == H5I_INVALID_HID)
            return -1;
        hsize_t start[] = {currentDims[0], 0};
        hsize_t count[] = {length, currentDims[1]};
        if (H5Sselect_hyperslab(filespaceID, H5S_SELECT_SET, start, NULL, count, NULL) < 0)
            return -1;

        auto memspaceID = H5Screate_simple(2, count, NULL);
        if (memspaceID == H5I_INVALID_HID) {
            H5Sclose(filespaceID);
            return -1;
        }

        // write the buffer to the extended region of the dataset
        status = H5Dwrite(
            this->m_dataset,
            this->m_datatype,
            memspaceID,
            filespaceID,
            H5P_DEFAULT,
            this->m_buffer.data()
        );
        H5Sclose(filespaceID);
        H5Sclose(memspaceID);
        this->m_buffer.clear();
        return status;
    }

    bool m_valid;
    std::vector<T> m_buffer;
    hid_t m_dataset;
    hid_t m_datatype;
};


class DataSet2D : public DataSet<double>
{
public:
    DataSet2D(hid_t fileID, const std::string& name)
        : DataSet{fileID, name + ".twodimen", 2, H5T_IEEE_F64LE}
    {}

    int write(double x, double y)
    {
        this->m_buffer.push_back(x);
        this->m_buffer.push_back(y);
        if (this->m_buffer.size() == (2 * 4096))
            return this->writeToDataset();
        return 0;
    }
    int write(const std::vector<double>& x, const std::vector<double>& y)
    {
        auto min = x.size() < y.size() ? x.size() : y.size();
        for (std::size_t i = 0; i < min; ++i) {
            auto status = this->write(x[i], y[i]);
            if (status != 0) return status;
        }
        return 0;
    }
};


typedef struct
{
    int x;
    int y;
    double value;
} CMData;


class DataSetCM : public DataSet<CMData>
{
public:
    DataSetCM(hid_t fileID, const std::string& name);

    int write(int x, int y, double value)
    {
        this->m_buffer.push_back({
            .x = x,
            .y = y,
            .value = value
        });
        if (this->m_buffer.size() >= 4096)
            return this->writeToDataset();
        return 0;
    }
    int write(int y, const std::vector<double>& row)
    {
        int x = 0;
        for (const auto& value : row) {
            auto status = this->write(x++, y, value);
            if (status != 0) return status;
        }
        return 0;
    }
    int write(const std::vector<std::vector<double>>& frame)
    {
        int y = 0;
        for (const auto& row : frame) {
            auto status = this->write(y++, row);
            if (status != 0) return status;
        }
        return 0;
    }
};


class DataSetGroup
{
public:
    DataSetGroup(hid_t fileID, const std::string& name)
        : m_dataset2D{new DataSet2D{fileID, name}}
        , m_datasetCM{new DataSetCM{fileID, name}}
    {}
    std::unique_ptr<DataSet2D>& dataset2D() { return this->m_dataset2D; }
    std::unique_ptr<DataSetCM>& datasetCM() { return this->m_datasetCM; }

private:
    std::unique_ptr<DataSet2D> m_dataset2D;
    std::unique_ptr<DataSetCM> m_datasetCM;
};


class DataManager
{
public:
    DataManager()
        : m_fileID{H5I_INVALID_HID}
    {}

    ~DataManager()
    {
        this->m_datasets.clear();
        if (this->m_fileID != H5I_INVALID_HID)
            H5Fclose(this->m_fileID);
    }

    void reset(const std::string& filename, std::size_t datasets)
    {
        this->m_datasets.clear();
        if (this->m_fileID != H5I_INVALID_HID)
            H5Fclose(this->m_fileID);

        this->m_fileID = H5Fcreate(filename.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
        if (this->m_fileID == H5I_INVALID_HID)
            throw std::runtime_error{"failed to create HDF5 file"};

        for (std::size_t i = 0; i < datasets; ++i) {
            auto datasetName = std::string{"dataset"} + std::to_string(i);
            this->m_datasets.push_back(DataSetGroup{this->m_fileID, datasetName});
        }
    }

    int write(std::size_t dataset, double x, double y)
    {
        return this->m_datasets.at(dataset).dataset2D()->write(x, y);
    }
    int write(std::size_t dataset, const std::vector<double>& x, const std::vector<double>& y)
    {
        return this->m_datasets.at(dataset).dataset2D()->write(x, y);
    }
    int write(std::size_t dataset, int x, int y, double value)
    {
        return this->m_datasets.at(dataset).datasetCM()->write(x, y, value);
    }
    int write(std::size_t dataset, int y, const std::vector<double>& row)
    {
        return this->m_datasets.at(dataset).datasetCM()->write(y, row);
    }
    int write(std::size_t dataset, const std::vector<std::vector<double>>& frame)
    {
        return this->m_datasets.at(dataset).datasetCM()->write(frame);
    }

    int flush(std::size_t dataset)
    {
        auto status = 0;

        status = this->m_datasets.at(dataset).dataset2D()->flush();
        if (status < 0) return status;

        status = this->m_datasets.at(dataset).datasetCM()->flush();
        return status;
    }

private:
    std::vector<DataSetGroup> m_datasets;
    hid_t m_fileID;
};
