#include "datamanager.hpp"


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
