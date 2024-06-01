/*
 * ExaPlot
 * data management unit
 * 
 * SPDX-License-Identifier: GPL-3.0
 * Copyright (C) 2024 bytemarx
 */

#pragma once

#include "hdf5.h"

#include <QObject>

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <filesystem>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "dataconfig.hpp"


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
            try {
                this->flush();
            } catch (std::runtime_error const& e) {
                std::cerr << "Failed to flush dataset during destruction: " << e.what() << '\n';
            }
            H5Dclose(this->m_dataset);
        }
    }

    void flush()
    {
        this->writeToDataset();
        if (H5Dflush(this->m_dataset) < 0)
            throw std::runtime_error{"failed to flush dataset"};
    }

protected:
    void writeToDataset()
    {
        if (this->m_buffer.size() == 0)
            return;

        auto dataspaceID = H5Dget_space(this->m_dataset);
        if (dataspaceID == H5I_INVALID_HID)
            throw std::runtime_error{"failed to get dataspace (initial)"};

        // retrieve current dataset dimensions
        hsize_t currentDims[2] = {0};
        int ndims = H5Sget_simple_extent_dims(dataspaceID, currentDims, NULL);
        H5Sclose(dataspaceID);
        if (ndims < 0)
            throw std::runtime_error{"failed to retrieve dataset dimensions"};

        hsize_t length = static_cast<hsize_t>(this->m_buffer.size()) / currentDims[1];

        // extend the dataset
        hsize_t updatedDims[2] = {currentDims[0] + length, currentDims[1]};
        if (H5Dset_extent(this->m_dataset, updatedDims) < 0)
            throw std::runtime_error{"failed to extend dataset"};

        // select the extended region
        dataspaceID = H5Dget_space(this->m_dataset);
        if (dataspaceID == H5I_INVALID_HID)
            throw std::runtime_error{"failed to get dataspace (extended)"};
        hsize_t start[] = {currentDims[0], 0};
        hsize_t count[] = {length, currentDims[1]};
        if (H5Sselect_hyperslab(dataspaceID, H5S_SELECT_SET, start, NULL, count, NULL) < 0)
            throw std::runtime_error{"failed to select extended dataspace"};

        auto memspaceID = H5Screate_simple(2, count, NULL);
        if (memspaceID == H5I_INVALID_HID) {
            H5Sclose(dataspaceID);
            throw std::runtime_error{"failed to create memory dataspace"};
        }

        // write the buffer to the extended region of the dataset
        auto status = H5Dwrite(
            this->m_dataset,
            this->m_datatype,
            memspaceID,
            dataspaceID,
            H5P_DEFAULT,
            this->m_buffer.data()
        );

        // TODO: check close returns?
        H5Sclose(dataspaceID);
        H5Sclose(memspaceID);
        if (status < 0)
            throw std::runtime_error{"failed to write buffer to dataset"};
        this->m_buffer.clear();
    }

    bool m_valid;
    std::vector<T> m_buffer;
    hid_t m_dataset;
    hid_t m_datatype;
};


class DataSet2D : public DataSet<double>
{
public:
    DataSet2D(hid_t fileID, const std::string& name);

    void write(double x, double y);
    void write(const std::vector<double>& x, const std::vector<double>& y);
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

    void write(int x, int y, double value);
    void write(int y, const std::vector<double>& row);
    void write(const std::vector<std::vector<double>>& frame);
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


class DataManager : public QObject
{
    Q_OBJECT

public:
    DataManager();
    ~DataManager();

Q_SIGNALS:
    void error(const QString&);
    void resetCompleted(bool, const QString&);
    void opened(bool, const QString&);
    void closed(bool, const QString&);

public Q_SLOTS:
    void configure(const exa::DatafileConfig& config);
    void open(const std::filesystem::path& path, std::size_t datasets);
    void close();

    void write2D(std::size_t plotIdx, double x, double y);
    void write2DVec(std::size_t plotIdx, const std::vector<double>& x, const std::vector<double>& y);

    void writeCM(std::size_t plotIdx, int x, int y, double value);
    void writeCMVec(std::size_t plotIdx, int y, const std::vector<double>& row);
    void writeCMFrame(std::size_t plotIdx, const std::vector<std::vector<double>>& frame);

    void flush(std::size_t plotIdx);

private:
    void setEnabled(bool enabled);

    bool m_enabled;
    std::vector<DataSetGroup> m_datasets;
    hid_t m_fileID;
};
