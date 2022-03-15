# -*- coding: utf-8 -*-
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: proto/hms_and_cmd_data.proto

from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from google.protobuf import reflection as _reflection
from google.protobuf import symbol_database as _symbol_database
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()




DESCRIPTOR = _descriptor.FileDescriptor(
  name='proto/hms_and_cmd_data.proto',
  package='',
  syntax='proto3',
  serialized_options=None,
  create_key=_descriptor._internal_create_key,
  serialized_pb=b'\n\x1cproto/hms_and_cmd_data.proto\"\xc7\x03\n\x07HmsData\x12\x16\n\x0e\x62\x61tteryVoltage\x18\x01 \x01(\x02\x12\x0e\n\x06nCells\x18\x02 \x01(\r\x12\'\n\x0cmainLogLevel\x18\x03 \x01(\x0e\x32\x11.HmsData.LogLevel\x12*\n\x0fsensorsLogLevel\x18\x04 \x01(\x0e\x32\x11.HmsData.LogLevel\x12&\n\x0bnavLogLevel\x18\x05 \x01(\x0e\x32\x11.HmsData.LogLevel\x12+\n\x10guidanceLogLevel\x18\x06 \x01(\x0e\x32\x11.HmsData.LogLevel\x12\x17\n\x0fnetworkTickRate\x18\x07 \x01(\r\x12\x14\n\x0cmainTickRate\x18\x08 \x01(\r\x12\x17\n\x0fsensorsTickRate\x18\t \x01(\r\x12\x18\n\x10\x63ombinedTickRate\x18\n \x01(\r\x12\x1b\n\x13longestCombinedTick\x18\x0b \x01(\r\x12\x11\n\terrorInfo\x18\x0c \x01(\t\"\'\n\x05\x45rror\x12\x0f\n\x0bLOW_BATTERY\x10\x00\x12\r\n\tWTF_AHMAD\x10\x01\"/\n\x08LogLevel\x12\n\n\x06NORMAL\x10\x00\x12\t\n\x05\x44\x45\x42UG\x10\x01\x12\x0c\n\x08OVERKILL\x10\x02\"\xae\x04\n\x07\x43mdData\x12\x13\n\x0bplaceholder\x18\x01 \x01(\x05\x12\x11\n\tleftPower\x18\x02 \x01(\x02\x12\x12\n\nrightPower\x18\x03 \x01(\x02\x12\x11\n\tpropPower\x18\x04 \x01(\x02\x12#\n\x08runState\x18\x05 \x01(\x0e\x32\x11.CmdData.RunState\x12\'\n\x0cmainLogLevel\x18\x06 \x01(\x0e\x32\x11.HmsData.LogLevel\x12*\n\x0fsensorsLogLevel\x18\x07 \x01(\x0e\x32\x11.HmsData.LogLevel\x12&\n\x0bnavLogLevel\x18\x08 \x01(\x0e\x32\x11.HmsData.LogLevel\x12+\n\x10guidanceLogLevel\x18\t \x01(\x0e\x32\x11.HmsData.LogLevel\x12\x0e\n\x06nTraps\x18\n \x01(\r\x12\r\n\x05trapX\x18\x0b \x03(\x02\x12\r\n\x05trapY\x18\x0c \x03(\x02\x12\x0f\n\x07simPosX\x18\r \x01(\x02\x12\x0f\n\x07simPosY\x18\x0e \x01(\x02\x12\x0f\n\x07simVelX\x18\x0f \x01(\x02\x12\x0f\n\x07simVelY\x18\x10 \x01(\x02\x12\x0f\n\x07simAccX\x18\x11 \x01(\x02\x12\x0f\n\x07simAccY\x18\x12 \x01(\x02\x12\x10\n\x08simAngXy\x18\x13 \x01(\x02\x12\x13\n\x0bsimAngVelXy\x18\x14 \x01(\x02\x12\x13\n\x0bsimAngAccXy\x18\x15 \x01(\x02\"5\n\x08RunState\x12\n\n\x06\x45_STOP\x10\x00\x12\x08\n\x04\x41UTO\x10\x01\x12\n\n\x06TELEOP\x10\x02\x12\x07\n\x03SIM\x10\x03\x62\x06proto3'
)



_HMSDATA_ERROR = _descriptor.EnumDescriptor(
  name='Error',
  full_name='HmsData.Error',
  filename=None,
  file=DESCRIPTOR,
  create_key=_descriptor._internal_create_key,
  values=[
    _descriptor.EnumValueDescriptor(
      name='LOW_BATTERY', index=0, number=0,
      serialized_options=None,
      type=None,
      create_key=_descriptor._internal_create_key),
    _descriptor.EnumValueDescriptor(
      name='WTF_AHMAD', index=1, number=1,
      serialized_options=None,
      type=None,
      create_key=_descriptor._internal_create_key),
  ],
  containing_type=None,
  serialized_options=None,
  serialized_start=400,
  serialized_end=439,
)
_sym_db.RegisterEnumDescriptor(_HMSDATA_ERROR)

_HMSDATA_LOGLEVEL = _descriptor.EnumDescriptor(
  name='LogLevel',
  full_name='HmsData.LogLevel',
  filename=None,
  file=DESCRIPTOR,
  create_key=_descriptor._internal_create_key,
  values=[
    _descriptor.EnumValueDescriptor(
      name='NORMAL', index=0, number=0,
      serialized_options=None,
      type=None,
      create_key=_descriptor._internal_create_key),
    _descriptor.EnumValueDescriptor(
      name='DEBUG', index=1, number=1,
      serialized_options=None,
      type=None,
      create_key=_descriptor._internal_create_key),
    _descriptor.EnumValueDescriptor(
      name='OVERKILL', index=2, number=2,
      serialized_options=None,
      type=None,
      create_key=_descriptor._internal_create_key),
  ],
  containing_type=None,
  serialized_options=None,
  serialized_start=441,
  serialized_end=488,
)
_sym_db.RegisterEnumDescriptor(_HMSDATA_LOGLEVEL)

_CMDDATA_RUNSTATE = _descriptor.EnumDescriptor(
  name='RunState',
  full_name='CmdData.RunState',
  filename=None,
  file=DESCRIPTOR,
  create_key=_descriptor._internal_create_key,
  values=[
    _descriptor.EnumValueDescriptor(
      name='E_STOP', index=0, number=0,
      serialized_options=None,
      type=None,
      create_key=_descriptor._internal_create_key),
    _descriptor.EnumValueDescriptor(
      name='AUTO', index=1, number=1,
      serialized_options=None,
      type=None,
      create_key=_descriptor._internal_create_key),
    _descriptor.EnumValueDescriptor(
      name='TELEOP', index=2, number=2,
      serialized_options=None,
      type=None,
      create_key=_descriptor._internal_create_key),
    _descriptor.EnumValueDescriptor(
      name='SIM', index=3, number=3,
      serialized_options=None,
      type=None,
      create_key=_descriptor._internal_create_key),
  ],
  containing_type=None,
  serialized_options=None,
  serialized_start=996,
  serialized_end=1049,
)
_sym_db.RegisterEnumDescriptor(_CMDDATA_RUNSTATE)


_HMSDATA = _descriptor.Descriptor(
  name='HmsData',
  full_name='HmsData',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  create_key=_descriptor._internal_create_key,
  fields=[
    _descriptor.FieldDescriptor(
      name='batteryVoltage', full_name='HmsData.batteryVoltage', index=0,
      number=1, type=2, cpp_type=6, label=1,
      has_default_value=False, default_value=float(0),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='nCells', full_name='HmsData.nCells', index=1,
      number=2, type=13, cpp_type=3, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='mainLogLevel', full_name='HmsData.mainLogLevel', index=2,
      number=3, type=14, cpp_type=8, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='sensorsLogLevel', full_name='HmsData.sensorsLogLevel', index=3,
      number=4, type=14, cpp_type=8, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='navLogLevel', full_name='HmsData.navLogLevel', index=4,
      number=5, type=14, cpp_type=8, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='guidanceLogLevel', full_name='HmsData.guidanceLogLevel', index=5,
      number=6, type=14, cpp_type=8, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='networkTickRate', full_name='HmsData.networkTickRate', index=6,
      number=7, type=13, cpp_type=3, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='mainTickRate', full_name='HmsData.mainTickRate', index=7,
      number=8, type=13, cpp_type=3, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='sensorsTickRate', full_name='HmsData.sensorsTickRate', index=8,
      number=9, type=13, cpp_type=3, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='combinedTickRate', full_name='HmsData.combinedTickRate', index=9,
      number=10, type=13, cpp_type=3, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='longestCombinedTick', full_name='HmsData.longestCombinedTick', index=10,
      number=11, type=13, cpp_type=3, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='errorInfo', full_name='HmsData.errorInfo', index=11,
      number=12, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=b"".decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
    _HMSDATA_ERROR,
    _HMSDATA_LOGLEVEL,
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=33,
  serialized_end=488,
)


_CMDDATA = _descriptor.Descriptor(
  name='CmdData',
  full_name='CmdData',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  create_key=_descriptor._internal_create_key,
  fields=[
    _descriptor.FieldDescriptor(
      name='placeholder', full_name='CmdData.placeholder', index=0,
      number=1, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='leftPower', full_name='CmdData.leftPower', index=1,
      number=2, type=2, cpp_type=6, label=1,
      has_default_value=False, default_value=float(0),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='rightPower', full_name='CmdData.rightPower', index=2,
      number=3, type=2, cpp_type=6, label=1,
      has_default_value=False, default_value=float(0),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='propPower', full_name='CmdData.propPower', index=3,
      number=4, type=2, cpp_type=6, label=1,
      has_default_value=False, default_value=float(0),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='runState', full_name='CmdData.runState', index=4,
      number=5, type=14, cpp_type=8, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='mainLogLevel', full_name='CmdData.mainLogLevel', index=5,
      number=6, type=14, cpp_type=8, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='sensorsLogLevel', full_name='CmdData.sensorsLogLevel', index=6,
      number=7, type=14, cpp_type=8, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='navLogLevel', full_name='CmdData.navLogLevel', index=7,
      number=8, type=14, cpp_type=8, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='guidanceLogLevel', full_name='CmdData.guidanceLogLevel', index=8,
      number=9, type=14, cpp_type=8, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='nTraps', full_name='CmdData.nTraps', index=9,
      number=10, type=13, cpp_type=3, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='trapX', full_name='CmdData.trapX', index=10,
      number=11, type=2, cpp_type=6, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='trapY', full_name='CmdData.trapY', index=11,
      number=12, type=2, cpp_type=6, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='simPosX', full_name='CmdData.simPosX', index=12,
      number=13, type=2, cpp_type=6, label=1,
      has_default_value=False, default_value=float(0),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='simPosY', full_name='CmdData.simPosY', index=13,
      number=14, type=2, cpp_type=6, label=1,
      has_default_value=False, default_value=float(0),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='simVelX', full_name='CmdData.simVelX', index=14,
      number=15, type=2, cpp_type=6, label=1,
      has_default_value=False, default_value=float(0),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='simVelY', full_name='CmdData.simVelY', index=15,
      number=16, type=2, cpp_type=6, label=1,
      has_default_value=False, default_value=float(0),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='simAccX', full_name='CmdData.simAccX', index=16,
      number=17, type=2, cpp_type=6, label=1,
      has_default_value=False, default_value=float(0),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='simAccY', full_name='CmdData.simAccY', index=17,
      number=18, type=2, cpp_type=6, label=1,
      has_default_value=False, default_value=float(0),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='simAngXy', full_name='CmdData.simAngXy', index=18,
      number=19, type=2, cpp_type=6, label=1,
      has_default_value=False, default_value=float(0),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='simAngVelXy', full_name='CmdData.simAngVelXy', index=19,
      number=20, type=2, cpp_type=6, label=1,
      has_default_value=False, default_value=float(0),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='simAngAccXy', full_name='CmdData.simAngAccXy', index=20,
      number=21, type=2, cpp_type=6, label=1,
      has_default_value=False, default_value=float(0),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
    _CMDDATA_RUNSTATE,
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=491,
  serialized_end=1049,
)

_HMSDATA.fields_by_name['mainLogLevel'].enum_type = _HMSDATA_LOGLEVEL
_HMSDATA.fields_by_name['sensorsLogLevel'].enum_type = _HMSDATA_LOGLEVEL
_HMSDATA.fields_by_name['navLogLevel'].enum_type = _HMSDATA_LOGLEVEL
_HMSDATA.fields_by_name['guidanceLogLevel'].enum_type = _HMSDATA_LOGLEVEL
_HMSDATA_ERROR.containing_type = _HMSDATA
_HMSDATA_LOGLEVEL.containing_type = _HMSDATA
_CMDDATA.fields_by_name['runState'].enum_type = _CMDDATA_RUNSTATE
_CMDDATA.fields_by_name['mainLogLevel'].enum_type = _HMSDATA_LOGLEVEL
_CMDDATA.fields_by_name['sensorsLogLevel'].enum_type = _HMSDATA_LOGLEVEL
_CMDDATA.fields_by_name['navLogLevel'].enum_type = _HMSDATA_LOGLEVEL
_CMDDATA.fields_by_name['guidanceLogLevel'].enum_type = _HMSDATA_LOGLEVEL
_CMDDATA_RUNSTATE.containing_type = _CMDDATA
DESCRIPTOR.message_types_by_name['HmsData'] = _HMSDATA
DESCRIPTOR.message_types_by_name['CmdData'] = _CMDDATA
_sym_db.RegisterFileDescriptor(DESCRIPTOR)

HmsData = _reflection.GeneratedProtocolMessageType('HmsData', (_message.Message,), {
  'DESCRIPTOR' : _HMSDATA,
  '__module__' : 'proto.hms_and_cmd_data_pb2'
  # @@protoc_insertion_point(class_scope:HmsData)
  })
_sym_db.RegisterMessage(HmsData)

CmdData = _reflection.GeneratedProtocolMessageType('CmdData', (_message.Message,), {
  'DESCRIPTOR' : _CMDDATA,
  '__module__' : 'proto.hms_and_cmd_data_pb2'
  # @@protoc_insertion_point(class_scope:CmdData)
  })
_sym_db.RegisterMessage(CmdData)


# @@protoc_insertion_point(module_scope)
