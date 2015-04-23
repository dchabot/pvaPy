#include "boost/python/class.hpp"
#include "boost/python/module.hpp"
#include "boost/python/def.hpp"
#include "boost/python/self.hpp"
#include "boost/python/other.hpp"
#include "boost/python/enum.hpp"
#include "boost/python/dict.hpp"
#include "boost/python/operators.hpp"
#include "boost/python/manage_new_object.hpp"
#include "boost/python/overloads.hpp"
#include "boost/python/exception_translator.hpp"
#include "boost/python/object.hpp"
#include "boost/python/docstring_options.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/operators.hpp"

#include "PvObject.h"
#include "PVAPYMonitor.h"
#include "PvProvider.h"
#include "PvScalar.h"
#include "PvType.h"
#include "PvBoolean.h"
#include "PvByte.h"
#include "PvUByte.h"
#include "PvShort.h"
#include "PvUShort.h"
#include "PvInt.h"
#include "PvUInt.h"
#include "PvLong.h"
#include "PvULong.h"
#include "PvFloat.h"
#include "PvDouble.h"
#include "PvString.h"
#include "PvScalarArray.h"

#include "PvAlarm.h"
#include "PvTimeStamp.h"

#include "NtType.h"

#include "Channel.h"
#include "RpcClient.h"
#include "PvaException.h"
#include "PvaExceptionTranslator.h"

using namespace epics::pvData;
using namespace epics::easyPVA;
using namespace epics::pvaPy;

// Overload macros
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(PvObjectGetBoolean, PvObject::getBoolean, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(PvObjectGetByte, PvObject::getByte, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(PvObjectGetUByte, PvObject::getUByte, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(PvObjectGetShort, PvObject::getShort, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(PvObjectGetUShort, PvObject::getUShort, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(PvObjectGetInt, PvObject::getInt, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(PvObjectGetUInt, PvObject::getUInt, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(PvObjectGetLong, PvObject::getLong, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(PvObjectGetULong, PvObject::getULong, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(PvObjectGetFloat, PvObject::getFloat, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(PvObjectGetDouble, PvObject::getDouble, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(PvObjectGetString, PvObject::getString, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(PvObjectGetScalarArray, PvObject::getScalarArray, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(PvObjectGetStructure, PvObject::getStructure, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(PvObjectGetStructureArray, PvObject::getStructureArray, 0, 1)


BOOST_PYTHON_MODULE(easypva)
{
    using namespace boost::python;
    scope().attr("__doc__") = "easypva module is a python wrapper for easyPVA and other `EPICS V4 <http://epics-pvdata.sourceforge.net>`_ C++ libraries."; 
    docstring_options local_docstring_options(true, true, false);

    //
    // Exceptions
    //
    register_exception_translator<PvaException>(PvaExceptionTranslator::translator)
        ;

    //
    // PvType
    //
    enum_<PvType::ScalarType>("PvType")
        .value("BOOLEAN", PvType::Boolean)
        .value("BYTE", PvType::Byte)
        .value("UBYTE", PvType::UByte)
        .value("SHORT", PvType::Short)
        .value("USHORT", PvType::UShort)
        .value("INT", PvType::Int)
        .value("UINT", PvType::UInt)
        .value("LONG", PvType::Long)
        .value("ULONG", PvType::ULong)
        .value("FLOAT", PvType::Float)
        .value("DOUBLE", PvType::Double)
        .value("STRING", PvType::String)
        .export_values()
        ;

    //
    // PvObject
    //
    class_<PvObject>("PvObject", 
            "PvObject represents a generic PV structure.\n\n"
            "**PvObject(structureDict)**\n\n"
            "\t:Parameter: *structureDict* (dict) - dictionary of key:value pairs describing the underlying PV structure in terms of field names and their types\n\n\t"
            "The dictionary key is a string (PV field name), and value is one of:\n\n"
            "\t- PVTYPE: scalar type, can be BOOLEAN, BYTE, UBYTE, SHORT, USHORT, INT, UINT, LONG, ULONG, FLOAT, DOUBLE, or STRING\n"
            "\t- [PVTYPE]: single element list representing scalar array\n"
            "\t- {key:value,…}: structure\n"
            "\t- [{key:value,…}]: single element list representing structure array\n\n"
            "\t::\n\n"
            "\t\tpv1 = PvObject({'anInt' : INT})\n\n"
            "\t\tpv2 = PvObject({'aShort' : SHORT, 'anUInt' : UINT, 'aString' : STRING})\n\n"
            "\t\tpv3 = PvObject({'aStringArray' : [STRING], 'aStruct' : {'aString2' : STRING, 'aBoolArray' : [BOOLEAN], 'aStruct2' : {'aFloat' : FLOAT, 'aString3' : [STRING]}}})\n\n"
            "\t\tpv4 = PvObject({'aStructArray' : [{'anInt' : INT, 'anInt2' : INT, 'aDouble' : DOUBLE}]})\n\n", 
            init<boost::python::dict>(args("structureDict")))

        .def(str(self))

        .def("set", 
            static_cast<void(PvObject::*)(const boost::python::dict&)>(&PvObject::set), args("valueDict"),
            "Populates PV structure fields from python dictionary.\n\n"
            ":Parameter: *valueDict* (dict) - dictionary of key:value pairs that correspond to PV structure field names and their values\n\n::\n\n"
            "    pv.set({'anUInt' : 1, 'aString' : 'my string example'})\n\n")

        .def("get", static_cast<boost::python::dict(PvObject::*)()const>(&PvObject::get), 
            "Retrieves PV structure as python dictionary.\n\n"
            ":Returns: python key:value dictionary representing current PV structure in terms of field names and their values\n\n::\n\n"
            "    valueDict = pv.get()\n\n")

        .def("setBoolean", static_cast<void(PvObject::*)(bool)>(&PvObject::setBoolean), args("value"),
            "Sets boolean value for the PV field named 'value'.\n\n:"
            "Parameter: *value* (bool) - boolean value that will be assigned to the field named 'value'\n\n::\n\n"
            "    pv.setBoolean(True)\n\n")

        .def("setBoolean", static_cast<void(PvObject::*)(const std::string&,bool)>(&PvObject::setBoolean), args("name", "value"),
            "Sets boolean value for the given PV field.\n\n"
            ":Parameter: *name* (str) - field name\n\n"
            ":Parameter: *value* (bool) - boolean value that will be assigned to the given field\n\n"
            "::\n\n"
            "    pv.setBoolean('aBoolean', True)\n\n")

        .def("getBoolean", static_cast<bool(PvObject::*)(const std::string&)const>(&PvObject::getBoolean), PvObjectGetBoolean(args("name='value'"),
            "Retrieves boolean value assigned to the given PV field.\n\n"
            ":Parameter: *name* (str) - field name\n\n"
            ":Returns: boolean stored in the given PV field\n\n"
            "::\n\n"
            "    value = pv.getBoolean('aBoolean')\n\n"))
 
        .def("setByte", static_cast<void(PvObject::*)(int8)>(&PvObject::setByte), args("value"),
            "Sets byte (character) value for the PV field named 'value'.\n\n"
            ":Parameter: *value* (str) - byte value that will be assigned to the field named 'value'\n\n"
            "::\n\n"
            "    pv.setByte('a')\n\n")

        .def("setByte", static_cast<void(PvObject::*)(const std::string&,int8)>(&PvObject::setByte), args("name", "value"),
            "Sets byte (character) value for the given PV field.\n\n"
            ":Parameter: *name* (str) - field name\n\n"
            ":Parameter: *value* (str) - byte value that will be assigned to the given field\n\n"
            "::\n\n"
            "    pv.setByte('aByte', 'a')\n\n")

        .def("getByte", static_cast<int8(PvObject::*)(const std::string&)const>(&PvObject::getByte), PvObjectGetByte(args("name='value'"),
            "Retrieves byte (character) value assigned to the given PV field.\n\n"
            ":Parameter: *name* (str) - field name\n\n"
            ":Returns: byte stored in the given PV field\n\n"
            "::\n\n"
            "    value = pv.getByte('aByte')\n\n"))

        .def("setUByte", static_cast<void(PvObject::*)(uint8)>(&PvObject::setUByte), args("value"),
            "Sets unsigned byte (character) value for the PV field named 'value'.\n\n"
            ":Parameter: *value* (str) - unsigned byte that will be assigned to the field named 'value'\n\n"
            "::\n\n"
            "    pv.setUByte('a')\n\n")

        .def("setUByte", static_cast<void(PvObject::*)(const std::string&,uint8)>(&PvObject::setUByte), args("name", "value"),
            "Sets unsigned byte (character) value for the given PV field.\n\n"
            ":Parameter: *name* (str) - field name\n\n"
            ":Parameter: *value* (str) - unsigned byte value that will be assigned to the given field\n\n"
            "::\n\n"
            "    pv.setByte('anUByte', 'a')\n\n")

        .def("getUByte", static_cast<uint8(PvObject::*)(const std::string&)const>(&PvObject::getUByte), PvObjectGetUByte(args("name='value'"),
            "Retrieves unsigned byte (character) value assigned to the given PV field.\n\n"
            ":Parameter: *name* (str) - field name\n\n"
            ":Returns: unsigned byte stored in the given PV field\n\n"
            "::\n\n"
            "    value = pv.getUByte('anUByte')\n\n"))

        .def("setShort", static_cast<void(PvObject::*)(int16)>(&PvObject::setShort), args("value"),
            "Sets short value for the PV field named 'value'.\n\n"
            ":Parameter: *value* (int) - short value that will be assigned to the field named 'value'\n\n::\n\n"
            "    pv.setShort(10)\n\n")

        .def("setShort", static_cast<void(PvObject::*)(const std::string&,int16)>(&PvObject::setShort), args("name", "value"),
            "Sets short value for the given PV field.\n\n"
            ":Parameter: *name* (str) - field name\n\n"
            ":Parameter: *value* (int) - short value that will be assigned to the given field\n\n"
            "::\n\n"
            "    pv.setShort('aShort', 10)\n\n")

        .def("getShort", static_cast<int16(PvObject::*)(const std::string&)const>(&PvObject::getShort), PvObjectGetShort(args("name='value'"),
            "Retrieves short value assigned to the given PV field.\n\n"
            ":Parameter: *name* (str) - field name\n\n"
            ":Returns: short stored in the given PV field\n\n"
            "::\n\n"
            "    value = pv.getShort('aShort')\n\n"))

        .def("setUShort", static_cast<void(PvObject::*)(uint16)>(&PvObject::setUShort), args("value"),
            "Sets unsigned short value for the PV field named 'value'.\n\n"
            ":Parameter: *value* (int) - unsigned short value that will be assigned to the field named 'value'\n\n::\n\n"
            "    pv.setUShort(10)\n\n")

        .def("setUShort", static_cast<void(PvObject::*)(const std::string&,uint16)>(&PvObject::setUShort), args("name", "value"),
            "Sets unsigned short value for the given PV field.\n\n"
            ":Parameter: *name* (str) - field name\n\n"
            ":Parameter: *value* (int) - unsigned short value that will be assigned to the given field\n\n"
            "::\n\n"
            "    pv.setUShort('anUShort', 10)\n\n")

        .def("getUShort", static_cast<uint16(PvObject::*)(const std::string&)const>(&PvObject::getUShort), PvObjectGetUShort(args("name='value'"),
            "Retrieves unsigned short value assigned to the given PV field.\n\n"
            ":Parameter: *name* (str) - field name\n\n:Returns: unsigned short stored in the given PV field\n\n::\n\n"
            "    value = pv.getUShort('anUShort')\n\n"))

        .def("setInt", static_cast<void(PvObject::*)(int32)>(&PvObject::setInt), args("value"),
            "Sets integer value for the PV field named 'value'.\n\n"
            ":Parameter: *value* (int) - integer value that will be assigned to the field named 'value'\n\n::\n\n"
            "    pv.setInt(10)\n\n")

        .def("setInt", static_cast<void(PvObject::*)(const std::string&,int32)>(&PvObject::setInt), args("name", "value"),
            "Sets integer value for the given PV field.\n\n"
            ":Parameter: *name* (str) - field name\n\n"
            ":Parameter: *value* (int) - integer value that will be assigned to the given field\n\n::\n\n"
            "    pv.setInt('anInt', 10)\n\n")

        .def("getInt", static_cast<int32(PvObject::*)(const std::string&)const>(&PvObject::getInt), PvObjectGetInt(args("name='value'"),
            "Retrieves integer value assigned to the given PV field.\n\n"
            ":Parameter: *name* (str) - field name\n\n"
            ":Returns: integer stored in the given PV field\n\n::\n\n"
            "    value = pv.getInt('anInt')\n\n"))

        .def("setUInt", static_cast<void(PvObject::*)(uint32)>(&PvObject::setUInt), args("value"),
            "Sets unsigned integer value for the PV field named 'value'.\n\n"
            ":Parameter: *value* (int) - unsigned integer value that will be assigned to the field named 'value'\n\n::\n\n"
            "    pv.setUInt(10)\n\n")

        .def("setUInt", static_cast<void(PvObject::*)(const std::string&,uint32)>(&PvObject::setUInt), args("name", "value"),
            "Sets unsigned integer value for the given PV field.\n\n"
            ":Parameter: *name* (str) - field name\n\n"
            ":Parameter: *value* (int) - unsigned integer value that will be assigned to the given field\n\n::\n\n"
            "    pv.setUInt('anInt', 10)\n\n")

        .def("getUInt", static_cast<uint32(PvObject::*)(const std::string&)const>(&PvObject::getUInt), PvObjectGetUInt(args("name='value'"),
             "Retrieves unsigned integer value assigned to the given PV field.\n\n"
            ":Parameter: *name* (str) - field name\n\n"
            ":Returns: unsigned integer stored in the given PV field\n\n::\n\n"
            "    value = pv.getUInt('anUInt')\n\n"))

        .def("setLong", 
            static_cast<void(PvObject::*)(int64)>(&PvObject::setLong), args("value"),
            "Sets long value for the PV field named 'value'.\n\n"
            ":Parameter: *value* (long) - long value that will be assigned to the field named 'value'\n\n::\n\n"
            "    pv.setLong(10L)\n\n")

        .def("setLong", static_cast<void(PvObject::*)(const std::string&,int64)>(&PvObject::setLong), args("name", "value"),
            "Sets long value for the given PV field.\n\n"
            ":Parameter: *name* (str) - field name\n\n"
            ":Parameter: *value* (long) - long value that will be assigned to the given field\n\n::\n\n"
            "    pv.setLong('aLong', 10L)\n\n")

        .def("getLong", static_cast<int64(PvObject::*)(const std::string&)const>(&PvObject::getLong), PvObjectGetLong(args("name='value'"),
             "Retrieves long value assigned to the given PV field.\n\n"
            ":Parameter: *name* (str) - field name\n\n"
            ":Returns: long stored in the given PV field\n\n::\n\n"
            "    value = pv.getLong('aLong')\n\n"))

        .def("setULong", static_cast<void(PvObject::*)(uint64)>(&PvObject::setULong), args("value"),
            "Sets unsigned long value for the PV field named 'value'.\n\n"
            ":Parameter: *value* (long) - unsigned long value that will be assigned to the field named 'value'\n\n::\n\n"
            "    pv.setULong(10L)\n\n")

        .def("setULong", static_cast<void(PvObject::*)(const std::string&,uint64)>(&PvObject::setULong), args("name", "value"),
            "Sets unsigned long value for the given PV field.\n\n"
            ":Parameter: *name* (str) - field name\n\n"
            ":Parameter: *value* (long) - unsigned long value that will be assigned to the given field\n\n::\n\n"
            "    pv.setULong('aLong', 10L)\n\n")

        .def("getULong", static_cast<uint64(PvObject::*)(const std::string&)const>(&PvObject::getULong), PvObjectGetULong(args("name='value'"),
             "Retrieves unsigned long value assigned to the given PV field.\n\n"
            ":Parameter: *name* (str) - field name\n\n"
            ":Returns: unsigned long stored in the given PV field\n\n::\n\n"
            "    value = pv.getULong('anULong')\n\n"))

        .def("setFloat", static_cast<void(PvObject::*)(float)>(&PvObject::setFloat), args("value"),
            "Sets float value for the PV field named 'value'.\n\n"
            ":Parameter: *value* (float) - float value that will be assigned to the field named 'value'\n\n::\n\n"
            "    pv.setFloat(10.0)\n\n")

        .def("setFloat", static_cast<void(PvObject::*)(const std::string&,float)>(&PvObject::setFloat), args("name", "value"),
            "Sets float value for the given PV field.\n\n"
            ":Parameter: *name* (str) - field name\n\n"
            ":Parameter: *value* (float) - float value that will be assigned to the given field\n\n::\n\n"
            "    pv.setFloat('aFloat', 10.0)\n\n")

        .def("getFloat", static_cast<float(PvObject::*)(const std::string&)const>(&PvObject::getFloat), PvObjectGetFloat(args("name='value'"),
             "Retrieves float value assigned to the given PV field.\n\n"
            ":Parameter: *name* (str) - field name\n\n"
            ":Returns: float stored in the given PV field\n\n"
            "::\n\n"
            "    value = pv.getFloat('aFloat')\n\n"))

        .def("setDouble", static_cast<void(PvObject::*)(double)>(&PvObject::setDouble),
            args("value"),
            "Sets double value for the PV field named 'value'.\n\n"
            ":Parameter: *value* (float) - double value that will be assigned to the field named 'value'\n\n"
            "::\n\n"
            "    pv.setDouble(10.0)\n\n")

        .def("setDouble", static_cast<void(PvObject::*)(const std::string&,double)>(&PvObject::setDouble), args("name", "value"),
            "Sets double value for the given PV field.\n\n"
            ":Parameter: *name* (str) - field name\n\n"
            ":Parameter: *value* (float) - double value that will be assigned to the given field\n\n"
            "::\n\n"
            "    pv.setDouble('aDouble', 10.0)\n\n")

        .def("getDouble", static_cast<double(PvObject::*)(const std::string&)const>(&PvObject::getDouble), PvObjectGetDouble(args("name='value'"),
             "Retrieves double value assigned to the given PV field.\n\n"
            ":Parameter: *name* (str) - field name\n\n"
            ":Returns: double stored in the given PV field\n\n"
            "::\n\n"
            "    value = pv.getDouble('aDouble')\n\n"))

        .def("setString", static_cast<void(PvObject::*)(const std::string&)>(&PvObject::setString), args("value"),
            "Sets string value for the PV field named 'value'.\n\n"
            ":Parameter: *value* (str) - string value that will be assigned to the field named 'value'\n\n"
            "::\n\n"
            "    pv.setString('string value')\n\n")

        .def("setString", static_cast<void(PvObject::*)(const std::string&,const std::string&)>(&PvObject::setString), args("name", "value"),
            "Sets string value for the given PV field.\n\n"
            ":Parameter: *name* (str) - field name\n\n"
            ":Parameter: *value* (str) - string value that will be assigned to the given field\n\n"
            "::\n\n"
            "    pv.setString('aString', 'string value')\n\n")
        
        .def("getString", static_cast<std::string(PvObject::*)(const std::string&)const>(&PvObject::getString), PvObjectGetString(args("name='value'"),
             "Retrieves string value assigned to the given PV field.\n\n"
            ":Parameter: *name* (str) - field name\n\n"
            ":Returns: string stored in the given PV field\n\n"
            "::\n\n"
            "    value = pv.getString('aString')\n\n"))

        .def("setScalarArray", static_cast<void(PvObject::*)(const boost::python::list&)>(&PvObject::setScalarArray), args("valueList"),
            "Sets scalar array for the PV field named 'value'.\n\n"
            ":Parameter: *valueList* (list) - list of scalar values that will be assigned to the field named 'value'\n\n"
            "::\n\n"
            "    pv.setScalarArray([0,1,2,3,4])\n\n")

        .def("setScalarArray", static_cast<void(PvObject::*)(const std::string&,const boost::python::list&)>(&PvObject::setScalarArray), args("name", "valueList"),
            "Sets scalar array for the given PV field.\n\n"
            ":Parameter: *name* (str) - field name\n\n"
            ":Parameter: *valueList* (list) - list of scalar values that will be assigned to the given field\n\n"
            "::\n\n"
            "    pv.setScalarArray('aScalarArray', [0,1,2,3,4])\n\n")

        .def("getScalarArray", static_cast<boost::python::list(PvObject::*)(const std::string&)const>(&PvObject::getScalarArray), PvObjectGetScalarArray(args("name='value'"),
             "Retrieves scalar array assigned to the given PV field.\n\n"
            ":Parameter: *name* (str) - field name\n\n"
            ":Returns: list of scalar values stored in the given PV field\n\n"
            "::\n\n"
            "    valueList = pv.getScalarArray('aScalarArray')\n\n"))

        .def("setStructure", 
            static_cast<void(PvObject::*)(const boost::python::dict&)>(&PvObject::setStructure),
            args("valueDict"),
            "Sets structure for the PV field named 'value'.\n\n"
            ":Parameter: *valueDict* (dict) - dictionary of key:value pairs for PV structure that will be assigned to the field named 'value'\n\n"
            "::\n\n"
            "    pv.setStructure({'aString' : 'string value', 'anInt' : 1, 'aFloat' : 1.1})\n\n")

        .def("setStructure", 
            static_cast<void(PvObject::*)(const std::string&,const boost::python::dict&)>(&PvObject::setStructure),
            args("name", "valueDict"),
            "Sets structure for the given PV field.\n\n"
            ":Parameter: *name* (str) - field name\n\n"
            ":Parameter: *valueDict* (dict) - dictionary of key:value pairs for PV structure that will be assigned to the given field\n\n"
            "::\n\n"
            "    pv.setStructure('aStruct', {'aString' : 'string value', 'anInt' : 1, 'aFloat' : 1.1})\n\n")

        .def("getStructure", 
            static_cast<boost::python::dict(PvObject::*)(const std::string&)const>(&PvObject::getStructure), 
            PvObjectGetStructure(args("name='value'"),
             "Retrieves structure assigned to the given PV field.\n\n"
            ":Parameter: *name* (str) - field name\n\n"
            ":Returns: dictionary of key:value pairs for PV structure stored in the given PV field\n\n"
            "::\n\n"
            "    valueDict = pv.getStructure('aStruct')\n\n"))

        .def("setStructureArray", 
            static_cast<void(PvObject::*)(const boost::python::list&)>(&PvObject::setStructureArray),
            args("dictList"),
            "Sets structure array for the PV field named 'value'.\n\n"
            ":Parameter: *dictList* (list) - list of dictionaries (describing PV structures) that will be assigned to the field named 'value'\n\n"
            "::\n\n"
            "    pv.setStructureArray([{'anInt' : 1, 'aFloat' : 1.1},{'anInt' : 2, 'aFloat' : 2.2},{'anInt' : 3, 'aFloat' : 3.3}])\n\n")

        .def("setStructureArray", 
            static_cast<void(PvObject::*)(const std::string&,const boost::python::list&)>(&PvObject::setStructureArray),
            args("name", "dictList"),
            "Sets structure array for the given PV field.\n\n"
            ":Parameter: *name* (str) - field name\n\n"
            ":Parameter: *dictList* (list) - list of dictionaries (describing PV structures) that will be assigned to the given field\n\n"
            "::\n\n"
            "    pv.setStructureArray('aStructArray', [{'anInt' : 1, 'aFloat' : 1.1},{'anInt' : 2, 'aFloat' : 2.2},{'anInt' : 3, 'aFloat' : 3.3}])\n\n")

        .def("getStructureArray", 
            static_cast<boost::python::list(PvObject::*)(const std::string&)const>(&PvObject::getStructureArray), 
            PvObjectGetStructureArray(args("name='value'"),
             "Retrieves structure array assigned to the given PV field.\n\n"
            ":Parameter: *name* (str) - field name\n\n"
            ":Returns: list of dictionaries (describing PV structures) stored in the given PV field\n\n"
            "::\n\n"
            "    dictList = pv.getStructureArray('aStructArray')\n\n"))

        .def("unionIsVariant",&PvObject::unionIsVariant,
            (arg("key") = "value"),
            "Is a field a variant union?\n"
            "arg\n"
            "    name fieldName = \"value\"\n"
            "return (True,False) if (is,is not)  a variant union\n"
            "throws InvalidArgument if field is not a union.\n\n"
            "example\n"
            "    value = pv.unionIsVariant()\n\n")

        .def("unionGetFieldNames",&PvObject::unionGetFieldNames,
            (arg("key") = "value"),
            "Get the field names for a regular union\n"
            "arg\n"
            "    name fieldName = \"value\"\n"
            "return list of union fieldnames. If variant union then empty list\n"
            "throws InvalidArgument if field is not a union.\n\n"
            "example\n"
            "    value = pv.unionGetFieldNames()\n\n")

        .def("unionSelect",&PvObject::unionSelect,
            (arg("key") = "value"),
            "Get a union with selected field name\n"
            "arg\n"
            "    fieldName The union field name.\n"
            "    name fieldName = \"value\"\n"
            "Returns PvObject.\n"
            "    The pvObject is a structure as follows:\n"
            "    structure\n"
            "        union value\n"
            "throws InvalidArgument if field is not a union.\n"
            "       ValueError if variant union or illegal union field name.\n\n"
            "example\n"
            "    value = pv.unionSelect(unionFieldName,fieldName)\n\n")

        .def("unionCreate",&PvObject::unionCreate,
            (arg("key") = "value"),
            "Create a union with selected field name\n"
            "arg\n"
            "    fieldName The union field name.\n"
            "    name fieldName = \"value\"\n"
            "Returns PvObject.\n"
            "    The pvObject is a structure as follows:\n"
            "    structure\n"
            "        union value\n"
            "throws InvalidArgument if field is not a union.\n"
            "       ValueError if variant union or illegal union field name.\n\n"
            "example\n"
            "    value = pv.unionCreate(unionFieldName,fieldName)\n\n")

        .def("unionGet",&PvObject::unionGet,
            (arg("key") = "value"),
            "Get a union with selected field name\n"
            "arg\n"
            "    name fieldName = \"value\"\n"
            "Returns PvObject.\n"
            "    The pvObject is a structure as follows:\n"
            "    structure\n"
            "        union value\n"
            "throws InvalidArgument if field is not a union.\n\n"
            "example\n"
            "    value = pv.unionGet(fieldName)\n\n")

        .def("unionSet",&PvObject::unionSet,
            (arg("key") = "value"),
            "Set the value of a union field\n"
            "arg\n"
            "    value a PvObject which must be a structure as follows:\n"
            "        structure\n"
            "            union value\n"
            "    name fieldName = \"value\"\n"
            "throws InvalidArgument if field is not a union.\n\n"
            "example\n"
            "    value = pv.unionSet(PvObject,fieldName)\n\n")

        .def("unionArrayIsVariant",&PvObject::unionArrayIsVariant,
            (arg("key") = "value"),
            "Is a field a variant union array?\n"
            "arg\n"
            "    name fieldName = \"value\"\n"
            "return (True,False) if (is,is not)  a variant union array\n"
            "throws InvalidArgument if field is not a union array.\n\n"
            "example\n"
            "    value = pv.unionArrayIsVariant()\n\n")

        .def("unionArrayGetFieldNames",&PvObject::unionArrayGetFieldNames,
            (arg("key") = "value"),
            "Get the field names for a regular union array\n"
            "arg\n"
            "    name fieldName = \"value\"\n"
            "return list of union fieldnames. If variant union then empty list\n"
            "throws InvalidArgument if field is not a union array.\n\n"
            "example\n"
            "    value = pv.unionArrayGetFieldNames()\n\n")


        .def("unionArrayCreateElement",&PvObject::unionArrayCreateElement,
            (arg("fieldName") = "",arg("key") = "value"),
            "Create a union with selected field name\n"
            "arg\n"
            "    fieldName The union field name.\n"
            "    name fieldName = \"value\"\n"
            "Returns PvObject.\n"
            "    The pvObject is a structure as follows:\n"
            "    structure\n"
            "        union value\n"
            "throws InvalidArgument if field is not a union.\n"
            "       ValueError if variant union or illegal union field name.\n\n"
            "example\n"
            "    value = pv.unionArrayCreateElement(unionFieldName,fieldName)\n\n")

	.def("unionArraySet", &PvObject::unionArraySet,
            (arg("key") = "value"),
            "Sets union array for the given PV field.\n"
            "arg\n"
            "    value a list of PvObjects. Each PvObject must be a structure as follows:\n"
            "        structure\n"
            "            union value\n"
            "    name fieldName = \"value\"\n"
            "throws InvalidArgument if field is not a union array.\n\n")

        .def("unionArrayGet", &PvObject::unionArrayGet,
            (arg("key") = "value"),
            "Get a union array with selected field name\n"
            "arg\n"
            "    name fieldName = \"value\"\n"
            "Returns a list of PvObjects.\n"
            "    Each pvObject is a structure as follows:\n"
            "    structure\n"
            "        union value\n"
            "throws InvalidArgument if field is not a union.\n\n"
            "example\n"
            "    value = pv.unionArrayGet(fieldName)\n\n")

        .def("toDict", 
            &PvObject::toDict,
            "Converts PV structure to python dictionary.\n\n"
            ":Returns: python key:value dictionary representing current PV structure in terms of field names and their values\n\n"
            "::\n\n"
            "    valueDict = pv.toDict()\n\n")

        .def("getStructureDict", 
            &PvObject::getStructureDict,
            "Retrieves PV structure definition as python dictionary.\n\n"
            ":Returns: python key:value dictionary representing PV structure definition in terms of field names and their types\n\n"
            "::\n\n"
            "    structureDict = pv.getStructureDict()\n\n")
        ;

    //
    // PV Scalar
    //
    class_<PvScalar, bases<PvObject> >("PvScalar",
             "PvScalar is a base class for all scalar PV types. It cannot be instantiated directly from python.\n\n",
              no_init)
        // use __int__ instead of .def(int_(self)) which requires operator::long()
        // use __float__ instead of .def(float_(self)) 
        .def("__int__", &PvScalar::toInt)   
        .def("__long__", &PvScalar::toLongLong)
        .def("__float__", &PvScalar::toDouble)   
        .def("__str__", &PvScalar::toString)   
        ;

    //
    // PV Boolean
    //
    class_<PvBoolean, bases<PvScalar> >("PvBoolean",
             "PvBoolean represents PV boolean type.\n\n"
             "**PvBoolean([value=False])**\n\n"
             "\t:Parameter: *value* (bool) - boolean value\n\n\t"
             "::\n\n"
             "\t\tpv = PvBoolean(True)\n\n",
              init<>())
        .def(init<bool>())
        .def("get", &PvBoolean::get,
             "Retrieves boolean PV value.\n\n"
             ":Returns: boolean value\n\n"
             "::\n\n"
             "    value = pv.get()\n\n")
        .def("set", &PvBoolean::set, args("value"),
             "Sets boolean PV value.\n\n"
             ":Parameter: *value* (bool) - boolean value\n\n"
             "::\n\n"
             "    pv.set(False)"
             "\n\n")
        ;

    //
    // PV Byte
    //
    class_<PvByte, bases<PvScalar> >("PvByte",
             "PvByte represents PV byte type.\n\n"
             "**PvByte([value=''])**\n\n"
             "\t:Parameter: *value* (str) - byte value\n\n"
             "\t::\n\n"
             "\t\tpv = PvByte('a')\n\n", init<>())
        .def(init<char>())
        .def("get", &PvByte::get,
             "Retrieves byte PV value.\n\n"
             ":Returns: byte value\n\n"
             "::\n\n"
             "    value = pv.get()\n\n")
        .def("set", &PvByte::set, args("value"),
             "Sets byte PV value.\n\n"
             ":Parameter: *value* (str) - byte value\n\n"
             "::\n\n"
             "    pv.set('a')\n\n")
        ;

    //
    // PV UByte
    //
    class_<PvUByte, bases<PvScalar> >("PvUByte",
             "PvUByte represents PV unsigned byte type.\n\n"
             "**PvUByte([value=0])**\n\n"
             "\t:Parameter: *value* (int) - unsigned byte value\n\n"
             "\t::\n\n"
             "\t\tpv = PvUByte(10)\n\n", init<>())
        .def(init<uint8>())
        .def("get", &PvUByte::get,
             "Retrieves unsigned byte PV value.\n\n"
             ":Returns: unsigned byte value\n\n"
             "::\n\n"
             "    value = pv.get()\n\n")
        .def("set", &PvUByte::set, args("value"),
             "Sets unsigned byte PV value.\n\n"
             ":Parameter: *value* (int) - unsigned byte value\n\n"
             "::\n\n"
             "    pv.set(10)\n\n")
        ;

    //
    // PV Short
    //
    class_<PvShort, bases<PvScalar> >("PvShort",
             "PvShort represents PV short type.\n\n"
             "**PvShort([value=0])**\n\n"
             "\t:Parameter: *value* (int) - short value\n\n"
             "\t::\n\n"
             "\t\tpv = PvShort(-10)\n\n", init<>())
        .def(init<short>())
        .def("get", &PvShort::get,
             "Retrieves short PV value.\n\n"
             ":Returns: short value\n\n"
             "::\n\n"
             "    value = pv.get()\n\n")
        .def("set", &PvShort::set, args("value"),
             "Sets short PV value.\n\n"
             ":Parameter: *value* (int) - short value\n\n"
             "::\n\n"
             "    pv.set(-10)\n\n")
        ;

    //
    // PV UShort
    //
    class_<PvUShort, bases<PvScalar> >("PvUShort",
             "PvUShort represents PV uint16 type.\n\n**PvUShort([value=0])**\n\n"
             "\t:Parameter: *value* (int) - unsigned short value\n\n"
             "\t::\n\n"
             "\t\tpv = PvUShort(10)\n\n", init<>())
        .def(init<uint16>())
        .def("get", &PvUShort::get,
             "Retrieves unsigned short PV value.\n\n"
             ":Returns: unsigned short value\n\n"
             "::\n\n"
             "    value = pv.get()\n\n")
        .def("set", &PvUShort::set, args("value"),
             "Sets unsigned short PV value.\n\n"
             ":Parameter: *value* (int) - unsigned short value\n\n"
             "::\n\n"
             "    pv.set(10)\n\n")
        ;

    //
    // PV Int
    //
    class_<PvInt, bases<PvScalar> >("PvInt",
             "PvInt represents PV integer type.\n\n"
             "**PvInt([value=0])**\n\n"
             "\t:Parameter: *value* (int) - integer value\n\n"
             "\t::\n\n"
             "\t\tpv = PvInt(-1000)\n\n", init<>())
        .def(init<int>())
        .def("get", &PvInt::get,
             "Retrieves integer PV value.\n\n"
             ":Returns: integer value\n\n"
             "::\n\n"
             "    value = pv.get()\n\n")
        .def("set", &PvInt::set, args("value"),
             "Sets integer PV value.\n\n"
             ":Parameter: *value* (int) - integer value\n\n"
             "::\n\n"
             "    pv.set(-1000)\n\n")
        ;

    //
    // PV UInt
    //
    class_<PvUInt, bases<PvScalar> >("PvUInt",
             "PvUInt represents PV uint32 type.\n\n"
             "**PvUInt([value=0])**\n\n"
             "\t:Parameter: *value* (int) - unsigned integer value\n\n"
             "\t::\n\n"
             "\t\tpv = PvUInt(1000)\n\n", init<>())
        .def(init<uint32>())
        .def("get", &PvUInt::get,  "Retrieves unsigned integer PV value.\n\n"
             ":Returns: unsigned integer value\n\n"
             "::\n\n"
             "    value = pv.get()\n\n")
        .def("set", &PvUInt::set, args("value"),
             "Sets unsigned integer PV value.\n\n"
             ":Parameter: *value* (int) - unsigned integer value\n\n"
             "::\n\n"
             "    pv.set(1000)\n\n")
        ;

    //
    // PV Long
    //
    class_<PvLong, bases<PvScalar> >("PvLong", 
            "PvLong represents PV long type.\n\n"
             "**PvLong([value=0])**\n\n"
             "\t:Parameter: *value* (long) - long value\n\n"
             "\t::\n\n"
             "\t\tpv = PvLong(-100000L)\n\n", init<>())
        .def(init<int64>())
        .def("get", &PvLong::get,
             "Retrieves long PV value.\n\n"
             ":Returns: long value\n\n"
             "::\n\n"
             "    value = pv.get()\n\n")
        .def("set", &PvLong::set,
             "Sets long PV value.\n\n"
             ":Parameter: *value* (long) - long value\n\n"
             "::\n\n"
             "    pv.set(-100000L)\n\n")
        ;

    //
    // PV ULong
    //
    class_<PvULong, bases<PvScalar> >("PvULong",
             "PvULong represents PV unsigned long type.\n\n"
             "**PvULong([value=0])**\n\n"
             "\t:Parameter: *value* (long) - unsigned long value\n\n"
             "\t::\n\n"
             "\t\tpv = PvULong(100000L)\n\n", init<>())
        .def(init<uint64>())
        .def("get", &PvULong::get,
             "Retrieves unsigned long PV value.\n\n"
             ":Returns: unsigned long value\n\n"
             "::\n\n"
             "    value = pv.get()\n\n")
        .def("set", &PvULong::set, args("value"),
             "Sets unsigned long PV value.\n\n"
             ":Parameter: *value* (long) - unsigned long value\n\n"
             "::\n\n"
             "    pv.set(100000L)\n\n")
        ;

    //
    // PV Float
    //
    class_<PvFloat, bases<PvScalar> >("PvFloat",
             "PvFloat represents PV float type.\n\n"
             "**PvFloat([value=0])**\n\n"
             "\t:Parameter: *value* (float) - float value\n\n"
             "\t::\n\n"
             "\t\tpv = PvFloat(1.1)\n\n", init<>())
        .def(init<float>())
        .def("get", &PvFloat::get,
             "Retrieves float PV value."
             "\n\n"
             ":Returns: float value\n\n"
             "::\n\n"
             "    value = pv.get()\n\n")
        .def("set", &PvFloat::set, args("value"),
             "Sets float PV value.\n\n"
             ":Parameter: *value* (float) - float value\n\n"
             "::\n\n"
             "    pv.set(1.1)\n\n")
        ;

    //
    // PV Double
    //
    class_<PvDouble, bases<PvScalar> >("PvDouble",
             "PvDouble represents PV double type.\n\n"
             "**PvDouble([value=0])**\n\n"
             "\t:Parameter: *value* (float) - double value\n\n"
             "\t::\n\n"
             "\t\tpv = PvDouble(1.1)\n\n", init<>())
        .def(init<double>())
        .def("get", &PvDouble::get,
             "Retrieves double PV value.\n\n"
             ":Returns: double value\n\n"
             "::\n\n"
             "    value = pv.get()\n\n")
        .def("set", &PvDouble::set, args("value"),
             "Sets double PV value.\n\n"
             ":Parameter: *value* (float) - double value\n\n"
             "::\n\n"
             "    pv.set(1.1)\n\n")
        ;

    //
    // PV String
    //
    class_<PvString, bases<PvScalar> >("PvString",
             "PvString represents PV string type.\n\n"
             "**PvString([value=''])**\n\n"
             "\t:Parameter: *value* (str) - string value\n\n"
             "\t::\n\n"
             "\t\tpv = PvString('stringValue')\n\n", init<>())
        .def(init<std::string>())
        .def("get", &PvString::get,
             "Retrieves string PV value.\n\n"
             ":Returns: string value\n\n"
             "::\n\n"
             "    value = pv.get()\n\n")
        .def("set", &PvString::set, args("value"),
             "Sets string PV value.\n\n"
             ":Parameter: *value* (str) - string value\n\n"
             "::\n\n"
             "    pv.set('stringValue')\n\n")
        ;

    //
    // PV Scalar Array
    //
    class_<PvScalarArray, bases<PvObject> >("PvScalarArray",
             "PvScalarArray represents PV scalar array.\n\n"
             "**PvScalarArray(scalarType)**\n\n"
             "\t:Parameter: *scalarType* (PVTYPE) - scalar type of array elements\n\n"
             "\t- PVTYPE: scalar type, can be BOOLEAN, BYTE, UBYTE, SHORT, USHORT, INT, UINT, LONG, ULONG, FLOAT, DOUBLE, or STRING\n\n"
             "\t::\n\n"
             "\t\tpv = PvScalarArray(INT)\n\n", init<PvType::ScalarType>())
        .def("get", &PvScalarArray::get,
             "Retrieves PV value list.\n\n"
             ":Returns: list of scalar values\n\n"
             "::\n\n"
             "    valueList = pv.get()\n\n")
        .def("set", &PvScalarArray::set, args("valueList"),
             "Sets PV value list.\n\n"
             ":Parameter: *valueList* (list) - list of scalar values\n\n"
             "::\n\n"
             "    pv.set([1,2,3,4,5])\n\n")
        .def("toList", &PvScalarArray::toList,
             "Converts PV to value list.\n\n"
             ":Returns: list of scalar values\n\n"
             "::\n\n"
             "    valueList = pv.toList()\n\n")
        ;

    //
    // PV TimeStamp
    //
    class_<PvTimeStamp, bases<PvObject> >("PvTimeStamp",
             "PvTimeStamp represents PV time stamp structure.\n\n"
             "**PvTimeStamp()**\n\n"
             "\t::\n\n"
             "\t\ttimestamp1 = PvTimeStamp()\n\n"
             "**PvTimeStamp(secondsPastEpoch, nanoseconds [, userTag=-1])**\n\n"
             "\t:Parameter: *secondsPastEpoch* (long) - seconds past epoch\n\n"
             "\t:Parameter: *nanoseconds* (int) - nanoseconds\n\n"
             "\t:Parameter: *userTag* (int) - user tag\n\n"
             "\t::\n\n"
             "\t\ttimeStamp2 = PvTimeStamp(1234567890, 10000)\n\n"
             "\t\ttimeStamp3 = PvTimeStamp(1234567890, 10000, 1)\n\n", init<>())
        .def(init<int64, int>())
        .def(init<int64, int, int>())
        .def("getSecondsPastEpoch", &PvTimeStamp::getSecondsPastEpoch,
             "Retrieves time stamp value for seconds past epoch.\n\n"
             ":Returns: seconds past epoch\n\n"
             "::\n\n"
             "    secondsPastEpoch = timeStamp.getSecondsPastEpoch()\n\n")
        .def("setSecondsPastEpoch", &PvTimeStamp::setSecondsPastEpoch, args("secondsPastEpoch"),
             "Sets time stamp value for seconds past epoch.\n\n"
             ":Parameter: *secondsPastEpoch* (long) - seconds past epoch\n\n"
             "::\n\n"
             "    timeStamp.setSecondsPastEpoch(1234567890)\n\n")
        .def("getNanoseconds", &PvTimeStamp::getNanoseconds,
             "Retrieves time stamp value for nanoseconds.\n\n"
             ":Returns: nanoseconds\n\n"
             "::\n\n"
             "    nanoseconds = timeStamp.getNanoseconds()\n\n")
        .def("setNanoseconds", &PvTimeStamp::setNanoseconds, args("nanoseconds"),
             "Sets time stamp value for nanoseconds.\n\n"
             ":Parameter: *nanoseconds* (int) - nanoseconds\n\n"
             "::\n\n"
             "    timeStamp.setNanoseconds(10000)\n\n")
        .def("getUserTag", &PvTimeStamp::getUserTag,
             "Retrieves user tag.\n\n"
             ":Returns: user tag\n\n"
             "::\n\n"
             "    userTag = timeStamp.getUserTag()\n\n")
        .def("setUserTag", &PvTimeStamp::setUserTag, args("userTag"),
             "Sets user tag.\n\n"
             ":Parameter: *userTag* (int) - user tag\n\n"
             "::\n\n"
             "    timeStamp.setUserTag(1)\n\n")
        ;

    //
    // PV Alarm
    //
    class_<PvAlarm, bases<PvObject> >("PvAlarm",
             "PvAlarm represents PV alarm structure.\n\n"
             "**PvAlarm()**\n\n"
             "\t::\n\n"
             "\t\talarm1 = PvAlarm()\n\n"
             "**PvAlarm(severity, status, message)**\n\n"
             "\t:Parameter: *severity* (int) - alarm severity\n\n"
             "\t:Parameter: *status* (int) - status code\n\n"
             "\t:Parameter: *message* (str) - alarm message\n\n"
             "\t::\n\n"
             "\t\talarm2 = PvAlarm(5, 1, 'alarm message')\n\n", init<>())
        .def(init<int, int, const std::string&>())
        .def("getSeverity", &PvAlarm::getSeverity,
             "Retrieves alarm severity.\n\n"
             ":Returns: alarm severity\n\n"
             "::\n\n"
             "    severity = alarm.getSeverity()\n\n")
        .def("setSeverity", &PvAlarm::setSeverity, args("severity"),
             "Sets alarm severity.\n\n"
             ":Parameter: *severity* (int) - alarm severity\n\n"
             "::\n\n"
             "    alarm.setSeverity(1)\n\n")
        .def("getStatus", &PvAlarm::getStatus,
             "Retrieves status code.\n\n"
             ":Returns: status code\n\n"
             "::\n\n"
             "    status = alarm.getStatusCode()\n\n")
        .def("setStatus", &PvAlarm::setStatus, args("status"),
             "Sets status code.\n\n"
             ":Parameter: *status* (int) - status code\n\n"
             "::\n\n"
             "    alarm.setStatus(1)\n\n")
        .def("getMessage", &PvAlarm::getMessage,
             "Retrieves alarm message.\n\n"
             ":Returns: alarm message\n\n"
             "::\n\n"
             "    message = alarm.getMessage()\n\n")
        .def("setMessage", &PvAlarm::setMessage, args("message"),
             "Sets alarm message.\n\n"
             ":Parameter: *message* (str) - alarm message\n\n"
             "::\n\n"
             "    alarm.setmessage('alarm message')\n\n")
        ;

    //
    // NT Type
    //
    class_<NtType, bases<PvObject> >("NtType",
             "NtType is a base class for all NT structures. It cannot be instantiated directly from python.\n\n", no_init)
        ;


    // Channel
    PvObject* (Channel::*get_request)(const std::string&) = &Channel::get;
    PvObject* (Channel::*get_default)() = &Channel::get;
    void (Channel::*put_obj_request)(const PvObject&,const std::string&) = &Channel::put;
    void (Channel::*put_obj)(const PvObject&) = &Channel::put;
    void (Channel::*put_string_request)(const std::string&,const std::string&) = &Channel::putString;
    void (Channel::*put_string)(const std::string&) = &Channel::putString;
    void (Channel::*put_list_request)(const boost::python::list&,const std::string&) = &Channel::put;
    void (Channel::*put_list)(const boost::python::list&) = &Channel::put;
    void (Channel::*put_bool)(bool) = &Channel::putBoolean;
    void (Channel::*put_bool_request)(bool,const std::string&) = &Channel::putBoolean;
    void (Channel::*put_byte)(int8) = &Channel::putByte;
    void (Channel::*put_byte_request)(int8,const std::string&) = &Channel::putByte;
    void (Channel::*put_ubyte)(uint8) = &Channel::putUByte;
    void (Channel::*put_ubyte_request)(uint8,const std::string&) = &Channel::putUByte;
    void (Channel::*put_short)(short) = &Channel::putShort;
    void (Channel::*put_short_request)(short,const std::string&) = &Channel::putShort;
    void (Channel::*put_ushort)(uint16) = &Channel::putUShort;
    void (Channel::*put_ushort_request)(uint16,const std::string&) = &Channel::putUShort;
    void (Channel::*put_int)(int) = &Channel::putInt;
    void (Channel::*put_int_request)(int,const std::string&) = &Channel::putInt;
    void (Channel::*put_uint)(uint32) = &Channel::putUInt;
    void (Channel::*put_uint_request)(uint32,const std::string&) = &Channel::putUInt;
    void (Channel::*put_long)(int64) = &Channel::putLong;
    void (Channel::*put_long_request)(int64,const std::string&) = &Channel::putLong;
    void (Channel::*put_ulong)(uint64) = &Channel::putULong;
    void (Channel::*put_ulong_request)(uint64,const std::string&) = &Channel::putULong;
    void (Channel::*put_float)(float) = &Channel::putFloat;
    void (Channel::*put_float_request)(float,const std::string&) = &Channel::putFloat;
    void (Channel::*put_double)(double) = &Channel::putDouble;
    void (Channel::*put_double_request)(double,const std::string&) = &Channel::putDouble;

    class_<Channel>("Channel",
             "Channel\n"
             "=======\n\n"
             "Provides access to easyChannel\n\n"
             "constructor\n"
             "    Channel(channelName)\n"
             "    Channel(channelName,providerName)\n"
             "        channelName string\n"
             "        providerName string\n"
             "           default is 'pva'\n\n"
             "example\n\n"
             "    c = Channel('double01')\n"
             "    print c.get()\n\n",
        init<std::string,std::string>())
        .def(init<std::string>())
        .def(init<std::string, std::string>())

        .def("get",get_request,
            return_value_policy<manage_new_object>(),
             "get data from the channel via a request string\n\n"
             "arg\n"
             "    request - a string describing the desired field\n"
             "        an example is 'value,alarm.timeStamp'\n\n"
             "return - A PvObject\n\n"
             "example\n"
             "    channel = Channel('enum01')\n"
             "    pv = channel.get('field(value.index)')\n"
             "    print pv\n"
             )

        .def("get", get_default,
            return_value_policy<manage_new_object>(),
             "get data from the channel with request = 'value'\n\n"
             "return - A PvObject\n\n"
             "example\n"
             "    channel = Channel('double01')\n"
             "    pv = channel.get()\n"
             "    print pv\n"
             )

        .def("put", put_obj_request,
             "put data to the channel via a PvObject.\n\n"
             "arg\n"
             "    pvobject - a PvObject\n"
             "    request - a string describing the desired field\n"
             "example\n"
             "    channel = Channel('enum01')\n\n"
             "    channel.put(PvInt(1), 'field(value.index)')\n\n"
             )

        .def("put", put_obj,
             "put data to the channel via a PvObject.\n\n"
             "arg\n"
             "    pvobject - a PvObject\n"
             "example\n"
             "    channel = Channel('double')\n\n"
             "    channel.put(PvDouble(1.0))\n\n"
             )

        .def("putString", put_string_request,
             "put data to the channel via a string.\n\n"
             "arg\n"
             "    value - string\n"
             "    request - a string describing the desired field\n"
             "example\n"
             "    channel = Channel('enum01')\n\n"
             "    channel.put('new value', 'field(value.index)')\n\n"
             )
        .def("put", put_string_request,
             "put data to the channel via a string.\n\n"
             "arg\n"
             "    value - string\n"
             "    request - a string describing the desired field\n"
             "example\n"
             "    channel = Channel('enum01')\n\n"
             "    channel.put('new value', 'field(value.index)')\n\n"
             )
        .def("putString", put_string,
             "put data to the channel via a string.\n\n"
             "arg\n"
             "    value - string\n"
             "    request - a string describing the desired field\n"
             "example\n"
             "    channel = Channel('enum01')\n\n"
             "    channel.put('new value', 'field(value.index)')\n\n"
             )
        .def("put", put_string,
             "put data to the channel via a string.\n\n"
             "arg\n"
             "    value - string\n"
             "    request - a string describing the desired field\n"
             "example\n"
             "    channel = Channel('enum01')\n\n"
             "    channel.put('new value', 'field(value.index)')\n\n"
             )
        .def("putScalarArray", put_list_request,
             "put data to an array channel via a list.\n\n"
             "arg\n"
             "    value - list\n"
             "    request - a string describing the desired field\n"
             )
        .def("put", put_list_request,
             "put data to an array channel via a list.\n\n"
             "arg\n"
             "    value - list\n"
             "    request - a string describing the desired field\n"
             )
        .def("putScalarArray", put_list,
             "put data to an array channel via a list.\n\n"
             "arg\n"
             "    value - list\n"
             )
        .def("put", put_list,
             "put data to an array channel via a list.\n\n"
             "arg\n"
             "    value - list\n"
             )

        .def("putBoolean", put_bool_request,
             "put data to a channel via a boolean.\n\n"
             "arg\n"
             "    value - True or False\n"
             "    request - a string describing the desired field\n"
             )
        .def("putBoolean", put_bool,
             "put data to a channel via a boolean.\n\n"
             "arg\n"
             "    value - True or False\n"
             )

        .def("putByte", put_byte_request,
             "put data to a channel via an 8 bit integer.\n\n"
             "arg\n"
             "    value - an 8 bit integer\n"
             "    request - a string describing the desired field\n"
             )
        .def("putByte", put_byte,
             "put data to a channel via an 8 bit integer.\n\n"
             "arg\n"
             "    value - an 8 bit integer\n"
             )

        .def("putUByte", put_ubyte_request,
             "put data to a channel via an unsigned 8 bit integer.\n\n"
             "arg\n"
             "    value - an unsigned 8 bit integer\n"
             "    request - a string describing the desired field\n"
             )
        .def("putUByte", put_ubyte,
             "put data to a channel via an unsigned 8 bit integer.\n\n"
             "arg\n"
             "    value - an unsigned 8 bit integer\n"
             )

        .def("putShort", put_short_request,
             "put data to a channel via an 16 bit integer.\n\n"
             "arg\n"
             "    value - an 16 bit integer\n"
             "    request - a string describing the desired field\n"
             )
        .def("putShort", put_short,
             "put data to a channel via an 16 bit integer.\n\n"
             "arg\n"
             "    value - an 16 bit integer\n"
             )
        .def("putUShort", put_ushort_request,
             "put data to a channel via an unsigned 16 bit integer.\n\n"
             "arg\n"
             "    value - an unsigned 16 bit integer\n"
             "    request - a string describing the desired field\n"
             )
        .def("putUShort", put_ushort,
             "put data to a channel via an unsigned 16 bit integer.\n\n"
             "arg\n"
             "    value - an unsigned 16 bit integer\n"
             )

        .def("putInt", put_int_request,
             "put data to a channel via an 32 bit integer.\n\n"
             "arg\n"
             "    value - an 32 bit integer\n"
             "    request - a string describing the desired field\n"
             )
        .def("putInt", put_int,
             "put data to a channel via a32 bit integer.\n\n"
             "arg\n"
             "    value - a32 bit integer\n"
             )

        .def("putUInt", put_uint_request,
             "put data to a channel via an unsigned 32 bit integer.\n\n"
             "arg\n"
             "    value - an unsigned 32 bit integer\n"
             "    request - a string describing the desired field\n"
             )
        .def("putUInt", put_uint,
             "put data to a channel via an unsigned 32 bit integer.\n\n"
             "arg\n"
             "    value - an unsigned 32 bit integer\n"
             )

        .def("putLong", put_long_request,
             "put data to a channel via a 64 bit integer.\n\n"
             "arg\n"
             "    value - an 64 bit integer\n"
             "    request - a string describing the desired field\n"
             )
        .def("putLong", put_long,
             "put data to a channel via a 64 bit integer.\n\n"
             "arg\n"
             "    value - an 64 bit integer\n"
             )

        .def("putULong", put_ulong_request,
             "put data to a channel via an unsigned 64 bit integer.\n\n"
             "arg\n"
             "    value - an unsigned 64 bit integer\n"
             "    request - a string describing the desired field\n"
             )
        .def("putULong", put_ulong,
             "put data to a channel via an unsigned 64 bit integer.\n\n"
             "arg\n"
             "    value - an unsigned 64 bit integer\n"
             )

        .def("putFloat", put_float_request,
             "put data to a channel via a float.\n\n"
             "arg\n"
             "    value - a float\n"
             "    request - a string describing the desired field\n"
             )
        .def("putFloat", put_float,
             "put data to a channel via a float.\n\n"
             "arg\n"
             "    value - a float\n"
             )

        .def("putDouble", put_double_request,
             "put data to a channel via a double.\n\n"
             "arg\n"
             "    value - a double\n"
             "    request - a string describing the desired field\n"
             )
        .def("putDouble", put_double,
             "put data to a channel via a double.\n\n"
             "arg\n"
             "    value - a double\n"
             )
        .def("put", put_double_request,
             "put data to a channel via a double.\n\n"
             "arg\n"
             "    value - a double\n"
             "    request - a string describing the desired field\n"
             )
        .def("put", put_double,
             "put data to a channel via a double.\n\n"
             "arg\n"
             "    value - a double\n"
             )


        .def("getTimeout", &Channel::getTimeout,
             "Retrieves channel timeout.\n\n"
             ":Returns: channel timeout in seconds\n\n"
             "::\n\n"
             "    timeout = channel.getTimeout()\n\n")
        .def("setTimeout", &Channel::setTimeout, args("timeout"),
             "Sets channel timeout.\n\n"
             ":Parameter: *timeout* (float) - channel timeout in seconds\n\n"
             "::\n\n"
             "    channel.setTimeout(10.0)\n\n")
        ;

        // Monitor
        class_<PVAPYMonitor>("Monitor",
             "Monitor\n"
             "=======\n\n"
             "Provides access to easyMonitor\n\n"
             "constructor\n"
             "    Monitor(channel)\n"
             "    Monitor(channel,request)\n"
             "    Monitor(channel,request,autoConnect)\n"
             "       channel - Must be connected Channel\n"
             "       request - string\n"
             "          example is \"value,alarm.timeStamp\"\n"
             "          default is \"value\"\n"
             "       autoStart - True or False\n"
             "          default is True\n"
             "\n",
           init<Channel>())

        .def(init<Channel,std::string>())
        .def(init<Channel,std::string,bool>())
        .def("start",&PVAPYMonitor::start,
            "start listening for monitors\n"
            "")
        .def("stop",&PVAPYMonitor::stop,
            "stop listening for monitors\n"
            "")
        .def("waitEvent",&PVAPYMonitor::waitEvent,
            args("secondsToWait"),
            (arg("secondsToWait") = 0.0),
            "wait for a monitor event\n\n"
            "arg\n"
            "   secondsToWait - number of seconds to wait for an event\n"
            "      a value of 0.0 means wait forever.\n\n"
            "return - True of False\n"
            "   If True then getData can be called.\n"
            "")
        .def("getData", &PVAPYMonitor::getData, 
            return_value_policy<manage_new_object>(),
            "get the data for the last sucessful waitEvent or poll\n"
            "return - True of False\n"
            "   If True then getData can be called.\n"
            "")
        .def("poll",&PVAPYMonitor::poll,
            "poll for a new event\n\n"
            "return - True of False\n"
            "   If True then getData can be called.\n"
            "")
        .def("releaseEvent",&PVAPYMonitor::releaseEvent,
            "release the event returned by the last poll or waitEvent\n"
            "   this must be called before again calling poll or waitEvent\n\n"
            "")
         ;

        // RPC Client
         class_<RpcClient>("RpcClient", "RpcClient is a client class for PVA RPC services.\n\n"
             "**RpcClient(channelName)**\n\n"
             "\t:Parameter: *channelName* (str) - RPC service channel name\n\n"
             "\tThis example creates RPC client for channel 'createNtTable':\n\n"
             "\t::\n\n"
             "\t\trpcClient = RpcClient('createNtTable')\n\n",
             init<std::string>())
        .def("invoke", &RpcClient::invoke, return_value_policy<manage_new_object>(), args("pvRequest"),
             "Invokes RPC call against service registered on the PV specified channel.\n\n"
             ":Parameter: *pvRequest* (PvObject) - PV request object with a structure conforming to requirements"
             " of the RPC service registered on the given PV channel\n\n"
             ":Returns: PV response object\n\n")
        ;


}