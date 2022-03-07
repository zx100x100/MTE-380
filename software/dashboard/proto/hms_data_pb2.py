# -*- coding: utf-8 -*-
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: proto/hms_data.proto

from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from google.protobuf import reflection as _reflection
from google.protobuf import symbol_database as _symbol_database
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()




DESCRIPTOR = _descriptor.FileDescriptor(
  name='proto/hms_data.proto',
  package='',
  syntax='proto3',
  serialized_options=None,
  create_key=_descriptor._internal_create_key,
  serialized_pb=b'\n\x14proto/hms_data.proto\"\xa9\x02\n\x07HmsData\x12\x16\n\x0e\x62\x61tteryVoltage\x18\x01 \x01(\x02\x12\x0e\n\x06nCells\x18\x02 \x01(\r\x12#\n\x08logLevel\x18\x03 \x01(\x0e\x32\x11.HmsData.LogLevel\x12\x17\n\x0fnetworkTickRate\x18\x04 \x01(\r\x12\x14\n\x0cmainTickRate\x18\x05 \x01(\r\x12\x18\n\x10\x63ombinedTickRate\x18\x06 \x01(\r\x12\x1b\n\x13longestCombinedTick\x18\x07 \x01(\r\x12\x11\n\terrorInfo\x18\x08 \x01(\t\"\'\n\x05\x45rror\x12\x0f\n\x0bLOW_BATTERY\x10\x00\x12\r\n\tOFF_TRACK\x10\x01\"/\n\x08LogLevel\x12\n\n\x06NORMAL\x10\x00\x12\t\n\x05\x44\x45\x42UG\x10\x01\x12\x0c\n\x08OVERKILL\x10\x02\x62\x06proto3'
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
      name='OFF_TRACK', index=1, number=1,
      serialized_options=None,
      type=None,
      create_key=_descriptor._internal_create_key),
  ],
  containing_type=None,
  serialized_options=None,
  serialized_start=234,
  serialized_end=273,
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
  serialized_start=275,
  serialized_end=322,
)
_sym_db.RegisterEnumDescriptor(_HMSDATA_LOGLEVEL)


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
      name='logLevel', full_name='HmsData.logLevel', index=2,
      number=3, type=14, cpp_type=8, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='networkTickRate', full_name='HmsData.networkTickRate', index=3,
      number=4, type=13, cpp_type=3, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='mainTickRate', full_name='HmsData.mainTickRate', index=4,
      number=5, type=13, cpp_type=3, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='combinedTickRate', full_name='HmsData.combinedTickRate', index=5,
      number=6, type=13, cpp_type=3, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='longestCombinedTick', full_name='HmsData.longestCombinedTick', index=6,
      number=7, type=13, cpp_type=3, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR,  create_key=_descriptor._internal_create_key),
    _descriptor.FieldDescriptor(
      name='errorInfo', full_name='HmsData.errorInfo', index=7,
      number=8, type=9, cpp_type=9, label=1,
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
  serialized_start=25,
  serialized_end=322,
)

_HMSDATA.fields_by_name['logLevel'].enum_type = _HMSDATA_LOGLEVEL
_HMSDATA_ERROR.containing_type = _HMSDATA
_HMSDATA_LOGLEVEL.containing_type = _HMSDATA
DESCRIPTOR.message_types_by_name['HmsData'] = _HMSDATA
_sym_db.RegisterFileDescriptor(DESCRIPTOR)

HmsData = _reflection.GeneratedProtocolMessageType('HmsData', (_message.Message,), {
  'DESCRIPTOR' : _HMSDATA,
  '__module__' : 'proto.hms_data_pb2'
  # @@protoc_insertion_point(class_scope:HmsData)
  })
_sym_db.RegisterMessage(HmsData)


# @@protoc_insertion_point(module_scope)
