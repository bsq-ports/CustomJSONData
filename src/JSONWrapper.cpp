// This is my most favorite file in this project

#include "JSONWrapper.h"

using namespace CustomJSONData;

DEFINE_TYPE(CustomJSONData, JSONWrapper);
DEFINE_TYPE(CustomJSONData, JSONWrapperUTF16);
DEFINE_TYPE(CustomJSONData, DocumentWrapper);

void DocumentWrapper::ctor() {
  INVOKE_CTOR();
}

void JSONWrapper::ctor() {
  INVOKE_CTOR();
}

JSONWrapper* JSONWrapper::GetCopy() {
  auto* copy = JSONWrapper::New_ctor();

  copy->value = value;
  copy->associatedData = associatedData;

  return copy;
}

void JSONWrapper::Init(std::optional<std::reference_wrapper<rapidjson::Value const>> value) {
  if (!value || !value->get().IsObject()) {
    this->value = std::nullopt;
    return;
  }

  this->value = value;
}

void JSONWrapperUTF16::ctor() {
  INVOKE_CTOR();
}

JSONWrapperUTF16* JSONWrapperUTF16::GetCopy() {
  auto* copy = JSONWrapperUTF16::New_ctor();

  copy->value = value;
  copy->associatedData = associatedData;

  return copy;
}

void JSONWrapperUTF16::Init(std::optional<std::reference_wrapper<SongCore::CustomJSONData::ValueUTF16 const>> value) {
  if (!value || !value->get().IsObject()) {
    this->value = std::nullopt;
    return;
  }

  this->value = value;
}