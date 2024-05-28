/*
 * ExaPlot
 * data management unit
 * 
 * SPDX-License-Identifier: GPL-3.0
 * Copyright (C) 2024 bytemarx
 */

#include "datamanager.hpp"

#include <ctime>


// static std::string
// datafile(const std::string& fmt)
// {
//     std::time_t result = std::time(nullptr);
//     std::asctime(std::localtime(&result));
//     return std::string{"data_"} + std::to_string(result) + ".hdf5";
// }


DataSet2D::DataSet2D(hid_t fileID, const std::string& name)
    : DataSet{fileID, name + ".twodimen", 2, H5T_IEEE_F64LE}
{
}


void
DataSet2D::write(double x, double y)
{
    this->m_buffer.push_back(x);
    this->m_buffer.push_back(y);
    if (this->m_buffer.size() == (2 * 4096))
        this->writeToDataset();
}


void
DataSet2D::write(const std::vector<double>& x, const std::vector<double>& y)
{
    auto min = x.size() < y.size() ? x.size() : y.size();
    for (std::size_t i = 0; i < min; ++i)
        this->write(x[i], y[i]);
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


void
DataSetCM::write(int x, int y, double value)
{
    this->m_buffer.push_back({
        .x = x,
        .y = y,
        .value = value
    });
    if (this->m_buffer.size() == 4096)
        this->writeToDataset();
}


void
DataSetCM::write(int y, const std::vector<double>& row)
{
    int x = 0;
    for (const auto& value : row)
        this->write(x++, y, value);
}


void
DataSetCM::write(const std::vector<std::vector<double>>& frame)
{
    int y = 0;
    for (const auto& row : frame)
        this->write(y++, row);
}


DataManager::DataManager()
    : QObject{nullptr}
    , m_enabled{false}
    , m_fileID{H5I_INVALID_HID}
{
}


DataManager::~DataManager()
{
    this->m_datasets.clear();
    if (this->m_fileID != H5I_INVALID_HID)
        H5Fclose(this->m_fileID);
}


void
DataManager::configure(const exa::DatafileConfig& config)
{
    if (config.enable)
        this->setEnabled(*config.enable);
}


void
DataManager::setEnabled(bool enabled)
{
    if (!enabled) {
        // put data manager in initial state
        this->m_datasets.clear();
        if (this->m_fileID != H5I_INVALID_HID)
            H5Fclose(this->m_fileID);
        this->m_fileID = H5I_INVALID_HID;
    }

    this->m_enabled = enabled;
}


void
DataManager::reset(const std::filesystem::path& path, std::size_t datasets)
{
    if (!this->m_enabled) {
        emit this->resetCompleted(false, "");
        return;
    }

    this->m_datasets.clear();
    if (this->m_fileID != H5I_INVALID_HID) {
        if (H5Fclose(this->m_fileID) < 0) {
            emit this->resetCompleted(true, "Error resetting data: failed to close HDF5 file");
            this->m_fileID = H5I_INVALID_HID;
            return;
        }
    }

    this->m_fileID = H5Fcreate(path.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    if (this->m_fileID == H5I_INVALID_HID) {
        emit this->resetCompleted(true, "Error resetting data: failed to create HDF5 file");
        return;
    }

    // TODO: zeroth/non-plot dataset
    try {
        for (std::size_t i = 1; i < datasets; ++i) {
            auto datasetName = std::string{"dataset"} + std::to_string(i);
            this->m_datasets.push_back(DataSetGroup{this->m_fileID, datasetName});
        }
    } catch (const std::runtime_error& e) {
        emit this->resetCompleted(true, QString{"Error resetting data: "}.append(e.what()));
        return;
    }

    emit this->resetCompleted(false, "");
}


void
DataManager::write2D(std::size_t plotIdx, double x, double y)
{
    if (!this->m_enabled) return;

    try {
        this->m_datasets.at(plotIdx).dataset2D()->write(x, y);
    } catch (const std::out_of_range& e) {
        emit this->error(QString{"Error writing data: plot index out of range"});
    } catch (const std::runtime_error& e) {
        emit this->error(QString{"Error writing data: "}.append(e.what()));
    }
}


void
DataManager::write2DVec(std::size_t plotIdx, const std::vector<double>& x, const std::vector<double>& y)
{
    if (!this->m_enabled) return;

    try {
        this->m_datasets.at(plotIdx).dataset2D()->write(x, y);
    } catch (const std::out_of_range& e) {
        emit this->error(QString{"Error writing data: plot index out of range"});
    } catch (const std::runtime_error& e) {
        emit this->error(QString{"Error writing data: "}.append(e.what()));
    }
}


void
DataManager::writeCM(std::size_t plotIdx, int x, int y, double value)
{
    if (!this->m_enabled) return;

    try {
        this->m_datasets.at(plotIdx).datasetCM()->write(x, y, value);
    } catch (const std::out_of_range& e) {
        emit this->error(QString{"Error writing data: plot index out of range"});
    } catch (const std::runtime_error& e) {
        emit this->error(QString{"Error writing data: "}.append(e.what()));
    }
}


void
DataManager::writeCMVec(std::size_t plotIdx, int y, const std::vector<double>& row)
{
    if (!this->m_enabled) return;

    try {
        this->m_datasets.at(plotIdx).datasetCM()->write(y, row);
    } catch (const std::out_of_range& e) {
        emit this->error(QString{"Error writing data: plot index out of range"});
    } catch (const std::runtime_error& e) {
        emit this->error(QString{"Error writing data: "}.append(e.what()));
    }
}


void
DataManager::writeCMFrame(std::size_t plotIdx, const std::vector<std::vector<double>>& frame)
{
    if (!this->m_enabled) return;

    try {
        this->m_datasets.at(plotIdx).datasetCM()->write(frame);
    } catch (const std::out_of_range& e) {
        emit this->error(QString{"Error writing data: plot index out of range"});
    } catch (const std::runtime_error& e) {
        emit this->error(QString{"Error writing data: "}.append(e.what()));
    }
}


void
DataManager::flush(std::size_t plotIdx)
{
    if (!this->m_enabled) return;

    try {
        this->m_datasets.at(plotIdx).dataset2D()->flush();
        this->m_datasets.at(plotIdx).datasetCM()->flush();
    } catch (const std::out_of_range& e) {
        emit this->error(QString{"Error flushing data: plot index out of range"});
    } catch (const std::runtime_error& e) {
        emit this->error(QString{"Error flushing data: "}.append(e.what()));
    }
}