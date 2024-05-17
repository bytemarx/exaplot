/*
 * ExaPlot
 * data management unit
 * 
 * SPDX-License-Identifier: GPL-3.0
 * Copyright (C) 2024 bytemarx
 */

#include "datamanager.hpp"


DataSet2D::DataSet2D(hid_t fileID, const std::string& name)
    : DataSet{fileID, name + ".twodimen", 2, H5T_IEEE_F64LE}
{
}


int
DataSet2D::write(double x, double y)
{
    this->m_buffer.push_back(x);
    this->m_buffer.push_back(y);
    if (this->m_buffer.size() == (2 * 4096))
        return this->writeToDataset();
    return 0;
}


int
DataSet2D::write(const std::vector<double>& x, const std::vector<double>& y)
{
    auto min = x.size() < y.size() ? x.size() : y.size();
    for (std::size_t i = 0; i < min; ++i) {
        auto status = this->write(x[i], y[i]);
        if (status != 0) return status;
    }
    return 0;
}


static hid_t
cmDatatype()
{
    auto datatype = H5Tcreate(H5T_COMPOUND, sizeof(CMData));
    H5Tinsert(datatype, "x", HOFFSET(CMData, x), H5T_NATIVE_INT);
    H5Tinsert(datatype, "y", HOFFSET(CMData, y), H5T_NATIVE_INT);
    H5Tinsert(datatype, "z", HOFFSET(CMData, value), H5T_NATIVE_DOUBLE);
    return datatype;
}


DataSetCM::DataSetCM(hid_t fileID, const std::string& name)
    : DataSet<CMData>{fileID, name + ".colormap", 1, cmDatatype()}
{
}


int
DataSetCM::write(int x, int y, double value)
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


int
DataSetCM::write(int y, const std::vector<double>& row)
{
    int x = 0;
    for (const auto& value : row) {
        auto status = this->write(x++, y, value);
        if (status != 0) return status;
    }
    return 0;
}


int
DataSetCM::write(const std::vector<std::vector<double>>& frame)
{
    int y = 0;
    for (const auto& row : frame) {
        auto status = this->write(y++, row);
        if (status != 0) return status;
    }
    return 0;
}


DataManager::DataManager()
    : m_fileID{H5I_INVALID_HID}
{
}


DataManager::~DataManager()
{
    this->m_datasets.clear();
    if (this->m_fileID != H5I_INVALID_HID)
        H5Fclose(this->m_fileID);
}


void
DataManager::reset(const std::string& filename, std::size_t datasets)
{
    this->m_datasets.clear();
    if (this->m_fileID != H5I_INVALID_HID)
        H5Fclose(this->m_fileID);

    this->m_fileID = H5Fcreate(filename.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    if (this->m_fileID == H5I_INVALID_HID)
        throw std::runtime_error{"failed to create HDF5 file"};

    // TODO: zeroth/non-plot dataset
    for (std::size_t i = 1; i < datasets; ++i) {
        auto datasetName = std::string{"dataset"} + std::to_string(i);
        this->m_datasets.push_back(DataSetGroup{this->m_fileID, datasetName});
    }
}


int
DataManager::write2D(std::size_t plotIdx, double x, double y)
{
    return this->m_datasets.at(plotIdx).dataset2D()->write(x, y);
}


int
DataManager::write2D(std::size_t plotIdx, const std::vector<double>& x, const std::vector<double>& y)
{
    return this->m_datasets.at(plotIdx).dataset2D()->write(x, y);
}


int
DataManager::writeCM(std::size_t plotIdx, int x, int y, double value)
{
    return this->m_datasets.at(plotIdx).datasetCM()->write(x, y, value);
}


int
DataManager::writeCM(std::size_t plotIdx, int y, const std::vector<double>& row)
{
    return this->m_datasets.at(plotIdx).datasetCM()->write(y, row);
}


int
DataManager::writeCM(std::size_t plotIdx, const std::vector<std::vector<double>>& frame)
{
    return this->m_datasets.at(plotIdx).datasetCM()->write(frame);
}


int
DataManager::flush(std::size_t plotIdx)
{
    auto status = 0;

    status = this->m_datasets.at(plotIdx).dataset2D()->flush();
    if (status < 0) return status;

    status = this->m_datasets.at(plotIdx).datasetCM()->flush();
    return status;
}
