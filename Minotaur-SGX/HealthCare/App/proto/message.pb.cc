// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: message.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "message.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/port.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace stream {
class MessageDefaultTypeInternal {
public:
 ::google::protobuf::internal::ExplicitlyConstructed<Message>
     _instance;
} _Message_default_instance_;

namespace protobuf_message_2eproto {


namespace {

::google::protobuf::Metadata file_level_metadata[1];

}  // namespace

PROTOBUF_CONSTEXPR_VAR ::google::protobuf::internal::ParseTableField
    const TableStruct::entries[] GOOGLE_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
  {0, 0, 0, ::google::protobuf::internal::kInvalidMask, 0, 0},
};

PROTOBUF_CONSTEXPR_VAR ::google::protobuf::internal::AuxillaryParseTableField
    const TableStruct::aux[] GOOGLE_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
  ::google::protobuf::internal::AuxillaryParseTableField(),
};
PROTOBUF_CONSTEXPR_VAR ::google::protobuf::internal::ParseTable const
    TableStruct::schema[] GOOGLE_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
  { NULL, NULL, 0, -1, -1, -1, -1, NULL, false },
};

const ::google::protobuf::uint32 TableStruct::offsets[] GOOGLE_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Message, _has_bits_),
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Message, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Message, value_),
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Message, gcm_tag_),
  GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Message, time_),
  0,
  1,
  2,
};
static const ::google::protobuf::internal::MigrationSchema schemas[] GOOGLE_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
  { 0, 8, sizeof(Message)},
};

static ::google::protobuf::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::google::protobuf::Message*>(&_Message_default_instance_),
};

namespace {

void protobuf_AssignDescriptors() {
  AddDescriptors();
  ::google::protobuf::MessageFactory* factory = NULL;
  AssignDescriptors(
      "message.proto", schemas, file_default_instances, TableStruct::offsets, factory,
      file_level_metadata, NULL, NULL);
}

void protobuf_AssignDescriptorsOnce() {
  static GOOGLE_PROTOBUF_DECLARE_ONCE(once);
  ::google::protobuf::GoogleOnceInit(&once, &protobuf_AssignDescriptors);
}

void protobuf_RegisterTypes(const ::std::string&) GOOGLE_ATTRIBUTE_COLD;
void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::internal::RegisterAllTypes(file_level_metadata, 1);
}

}  // namespace
void TableStruct::InitDefaultsImpl() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::internal::InitProtobufDefaults();
  _Message_default_instance_._instance.DefaultConstruct();
  ::google::protobuf::internal::OnShutdownDestroyMessage(
      &_Message_default_instance_);}

void InitDefaults() {
  static GOOGLE_PROTOBUF_DECLARE_ONCE(once);
  ::google::protobuf::GoogleOnceInit(&once, &TableStruct::InitDefaultsImpl);
}
namespace {
void AddDescriptorsImpl() {
  InitDefaults();
  static const char descriptor[] GOOGLE_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
      "\n\rmessage.proto\022\006stream\"7\n\007Message\022\r\n\005va"
      "lue\030\001 \001(\t\022\017\n\007gcm_tag\030\002 \001(\t\022\014\n\004time\030\003 \001(\004"
  };
  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
      descriptor, 80);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "message.proto", &protobuf_RegisterTypes);
}
} // anonymous namespace

void AddDescriptors() {
  static GOOGLE_PROTOBUF_DECLARE_ONCE(once);
  ::google::protobuf::GoogleOnceInit(&once, &AddDescriptorsImpl);
}
// Force AddDescriptors() to be called at dynamic initialization time.
struct StaticDescriptorInitializer {
  StaticDescriptorInitializer() {
    AddDescriptors();
  }
} static_descriptor_initializer;

}  // namespace protobuf_message_2eproto


// ===================================================================

#if !defined(_MSC_VER) || _MSC_VER >= 1900
const int Message::kValueFieldNumber;
const int Message::kGcmTagFieldNumber;
const int Message::kTimeFieldNumber;
#endif  // !defined(_MSC_VER) || _MSC_VER >= 1900

Message::Message()
  : ::google::protobuf::Message(), _internal_metadata_(NULL) {
  if (GOOGLE_PREDICT_TRUE(this != internal_default_instance())) {
    protobuf_message_2eproto::InitDefaults();
  }
  SharedCtor();
  // @@protoc_insertion_point(constructor:stream.Message)
}
Message::Message(const Message& from)
  : ::google::protobuf::Message(),
      _internal_metadata_(NULL),
      _has_bits_(from._has_bits_),
      _cached_size_(0) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  value_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  if (from.has_value()) {
    value_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.value_);
  }
  gcm_tag_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  if (from.has_gcm_tag()) {
    gcm_tag_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.gcm_tag_);
  }
  time_ = from.time_;
  // @@protoc_insertion_point(copy_constructor:stream.Message)
}

void Message::SharedCtor() {
  _cached_size_ = 0;
  value_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  gcm_tag_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  time_ = GOOGLE_ULONGLONG(0);
}

Message::~Message() {
  // @@protoc_insertion_point(destructor:stream.Message)
  SharedDtor();
}

void Message::SharedDtor() {
  value_.DestroyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  gcm_tag_.DestroyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}

void Message::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* Message::descriptor() {
  protobuf_message_2eproto::protobuf_AssignDescriptorsOnce();
  return protobuf_message_2eproto::file_level_metadata[kIndexInFileMessages].descriptor;
}

const Message& Message::default_instance() {
  protobuf_message_2eproto::InitDefaults();
  return *internal_default_instance();
}

Message* Message::New(::google::protobuf::Arena* arena) const {
  Message* n = new Message;
  if (arena != NULL) {
    arena->Own(n);
  }
  return n;
}

void Message::Clear() {
// @@protoc_insertion_point(message_clear_start:stream.Message)
  ::google::protobuf::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 3u) {
    if (cached_has_bits & 0x00000001u) {
      GOOGLE_DCHECK(!value_.IsDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited()));
      (*value_.UnsafeRawStringPointer())->clear();
    }
    if (cached_has_bits & 0x00000002u) {
      GOOGLE_DCHECK(!gcm_tag_.IsDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited()));
      (*gcm_tag_.UnsafeRawStringPointer())->clear();
    }
  }
  time_ = GOOGLE_ULONGLONG(0);
  _has_bits_.Clear();
  _internal_metadata_.Clear();
}

bool Message::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!GOOGLE_PREDICT_TRUE(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:stream.Message)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoffNoLastTag(127u);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // optional string value = 1;
      case 1: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(10u /* 10 & 0xFF */)) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_value()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
            this->value().data(), static_cast<int>(this->value().length()),
            ::google::protobuf::internal::WireFormat::PARSE,
            "stream.Message.value");
        } else {
          goto handle_unusual;
        }
        break;
      }

      // optional string gcm_tag = 2;
      case 2: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(18u /* 18 & 0xFF */)) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_gcm_tag()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
            this->gcm_tag().data(), static_cast<int>(this->gcm_tag().length()),
            ::google::protobuf::internal::WireFormat::PARSE,
            "stream.Message.gcm_tag");
        } else {
          goto handle_unusual;
        }
        break;
      }

      // optional uint64 time = 3;
      case 3: {
        if (static_cast< ::google::protobuf::uint8>(tag) ==
            static_cast< ::google::protobuf::uint8>(24u /* 24 & 0xFF */)) {
          set_has_time();
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint64, ::google::protobuf::internal::WireFormatLite::TYPE_UINT64>(
                 input, &time_)));
        } else {
          goto handle_unusual;
        }
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, _internal_metadata_.mutable_unknown_fields()));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:stream.Message)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:stream.Message)
  return false;
#undef DO_
}

void Message::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:stream.Message)
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  // optional string value = 1;
  if (cached_has_bits & 0x00000001u) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
      this->value().data(), static_cast<int>(this->value().length()),
      ::google::protobuf::internal::WireFormat::SERIALIZE,
      "stream.Message.value");
    ::google::protobuf::internal::WireFormatLite::WriteStringMaybeAliased(
      1, this->value(), output);
  }

  // optional string gcm_tag = 2;
  if (cached_has_bits & 0x00000002u) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
      this->gcm_tag().data(), static_cast<int>(this->gcm_tag().length()),
      ::google::protobuf::internal::WireFormat::SERIALIZE,
      "stream.Message.gcm_tag");
    ::google::protobuf::internal::WireFormatLite::WriteStringMaybeAliased(
      2, this->gcm_tag(), output);
  }

  // optional uint64 time = 3;
  if (cached_has_bits & 0x00000004u) {
    ::google::protobuf::internal::WireFormatLite::WriteUInt64(3, this->time(), output);
  }

  if (_internal_metadata_.have_unknown_fields()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        _internal_metadata_.unknown_fields(), output);
  }
  // @@protoc_insertion_point(serialize_end:stream.Message)
}

::google::protobuf::uint8* Message::InternalSerializeWithCachedSizesToArray(
    bool deterministic, ::google::protobuf::uint8* target) const {
  (void)deterministic; // Unused
  // @@protoc_insertion_point(serialize_to_array_start:stream.Message)
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  // optional string value = 1;
  if (cached_has_bits & 0x00000001u) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
      this->value().data(), static_cast<int>(this->value().length()),
      ::google::protobuf::internal::WireFormat::SERIALIZE,
      "stream.Message.value");
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        1, this->value(), target);
  }

  // optional string gcm_tag = 2;
  if (cached_has_bits & 0x00000002u) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
      this->gcm_tag().data(), static_cast<int>(this->gcm_tag().length()),
      ::google::protobuf::internal::WireFormat::SERIALIZE,
      "stream.Message.gcm_tag");
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        2, this->gcm_tag(), target);
  }

  // optional uint64 time = 3;
  if (cached_has_bits & 0x00000004u) {
    target = ::google::protobuf::internal::WireFormatLite::WriteUInt64ToArray(3, this->time(), target);
  }

  if (_internal_metadata_.have_unknown_fields()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields(), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:stream.Message)
  return target;
}

size_t Message::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:stream.Message)
  size_t total_size = 0;

  if (_internal_metadata_.have_unknown_fields()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        _internal_metadata_.unknown_fields());
  }
  if (_has_bits_[0 / 32] & 7u) {
    // optional string value = 1;
    if (has_value()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->value());
    }

    // optional string gcm_tag = 2;
    if (has_gcm_tag()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->gcm_tag());
    }

    // optional uint64 time = 3;
    if (has_time()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::UInt64Size(
          this->time());
    }

  }
  int cached_size = ::google::protobuf::internal::ToCachedSize(total_size);
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = cached_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void Message::MergeFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:stream.Message)
  GOOGLE_DCHECK_NE(&from, this);
  const Message* source =
      ::google::protobuf::internal::DynamicCastToGenerated<const Message>(
          &from);
  if (source == NULL) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:stream.Message)
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:stream.Message)
    MergeFrom(*source);
  }
}

void Message::MergeFrom(const Message& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:stream.Message)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = from._has_bits_[0];
  if (cached_has_bits & 7u) {
    if (cached_has_bits & 0x00000001u) {
      set_has_value();
      value_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.value_);
    }
    if (cached_has_bits & 0x00000002u) {
      set_has_gcm_tag();
      gcm_tag_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.gcm_tag_);
    }
    if (cached_has_bits & 0x00000004u) {
      time_ = from.time_;
    }
    _has_bits_[0] |= cached_has_bits;
  }
}

void Message::CopyFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:stream.Message)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void Message::CopyFrom(const Message& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:stream.Message)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool Message::IsInitialized() const {
  return true;
}

void Message::Swap(Message* other) {
  if (other == this) return;
  InternalSwap(other);
}
void Message::InternalSwap(Message* other) {
  using std::swap;
  value_.Swap(&other->value_);
  gcm_tag_.Swap(&other->gcm_tag_);
  swap(time_, other->time_);
  swap(_has_bits_[0], other->_has_bits_[0]);
  _internal_metadata_.Swap(&other->_internal_metadata_);
  swap(_cached_size_, other->_cached_size_);
}

::google::protobuf::Metadata Message::GetMetadata() const {
  protobuf_message_2eproto::protobuf_AssignDescriptorsOnce();
  return protobuf_message_2eproto::file_level_metadata[kIndexInFileMessages];
}

#if PROTOBUF_INLINE_NOT_IN_HEADERS
// Message

// optional string value = 1;
bool Message::has_value() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
void Message::set_has_value() {
  _has_bits_[0] |= 0x00000001u;
}
void Message::clear_has_value() {
  _has_bits_[0] &= ~0x00000001u;
}
void Message::clear_value() {
  value_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  clear_has_value();
}
const ::std::string& Message::value() const {
  // @@protoc_insertion_point(field_get:stream.Message.value)
  return value_.GetNoArena();
}
void Message::set_value(const ::std::string& value) {
  set_has_value();
  value_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:stream.Message.value)
}
#if LANG_CXX11
void Message::set_value(::std::string&& value) {
  set_has_value();
  value_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:stream.Message.value)
}
#endif
void Message::set_value(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  set_has_value();
  value_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:stream.Message.value)
}
void Message::set_value(const char* value, size_t size) {
  set_has_value();
  value_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:stream.Message.value)
}
::std::string* Message::mutable_value() {
  set_has_value();
  // @@protoc_insertion_point(field_mutable:stream.Message.value)
  return value_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
::std::string* Message::release_value() {
  // @@protoc_insertion_point(field_release:stream.Message.value)
  clear_has_value();
  return value_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
void Message::set_allocated_value(::std::string* value) {
  if (value != NULL) {
    set_has_value();
  } else {
    clear_has_value();
  }
  value_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set_allocated:stream.Message.value)
}

// optional string gcm_tag = 2;
bool Message::has_gcm_tag() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
void Message::set_has_gcm_tag() {
  _has_bits_[0] |= 0x00000002u;
}
void Message::clear_has_gcm_tag() {
  _has_bits_[0] &= ~0x00000002u;
}
void Message::clear_gcm_tag() {
  gcm_tag_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  clear_has_gcm_tag();
}
const ::std::string& Message::gcm_tag() const {
  // @@protoc_insertion_point(field_get:stream.Message.gcm_tag)
  return gcm_tag_.GetNoArena();
}
void Message::set_gcm_tag(const ::std::string& value) {
  set_has_gcm_tag();
  gcm_tag_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:stream.Message.gcm_tag)
}
#if LANG_CXX11
void Message::set_gcm_tag(::std::string&& value) {
  set_has_gcm_tag();
  gcm_tag_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:stream.Message.gcm_tag)
}
#endif
void Message::set_gcm_tag(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  set_has_gcm_tag();
  gcm_tag_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:stream.Message.gcm_tag)
}
void Message::set_gcm_tag(const char* value, size_t size) {
  set_has_gcm_tag();
  gcm_tag_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:stream.Message.gcm_tag)
}
::std::string* Message::mutable_gcm_tag() {
  set_has_gcm_tag();
  // @@protoc_insertion_point(field_mutable:stream.Message.gcm_tag)
  return gcm_tag_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
::std::string* Message::release_gcm_tag() {
  // @@protoc_insertion_point(field_release:stream.Message.gcm_tag)
  clear_has_gcm_tag();
  return gcm_tag_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
void Message::set_allocated_gcm_tag(::std::string* gcm_tag) {
  if (gcm_tag != NULL) {
    set_has_gcm_tag();
  } else {
    clear_has_gcm_tag();
  }
  gcm_tag_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), gcm_tag);
  // @@protoc_insertion_point(field_set_allocated:stream.Message.gcm_tag)
}

// optional uint64 time = 3;
bool Message::has_time() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
void Message::set_has_time() {
  _has_bits_[0] |= 0x00000004u;
}
void Message::clear_has_time() {
  _has_bits_[0] &= ~0x00000004u;
}
void Message::clear_time() {
  time_ = GOOGLE_ULONGLONG(0);
  clear_has_time();
}
::google::protobuf::uint64 Message::time() const {
  // @@protoc_insertion_point(field_get:stream.Message.time)
  return time_;
}
void Message::set_time(::google::protobuf::uint64 value) {
  set_has_time();
  time_ = value;
  // @@protoc_insertion_point(field_set:stream.Message.time)
}

#endif  // PROTOBUF_INLINE_NOT_IN_HEADERS

// @@protoc_insertion_point(namespace_scope)

}  // namespace stream

// @@protoc_insertion_point(global_scope)
