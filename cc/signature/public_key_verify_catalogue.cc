// Copyright 2017 Google Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
///////////////////////////////////////////////////////////////////////////////

#include "cc/signature/public_key_verify_catalogue.h"

#include "absl/strings/ascii.h"
#include "cc/catalogue.h"
#include "cc/signature/ecdsa_verify_key_manager.h"
#include "cc/key_manager.h"
#include "cc/util/status.h"
#include "cc/util/statusor.h"

namespace crypto {
namespace tink {

namespace {

crypto::tink::util::StatusOr<std::unique_ptr<KeyManager<PublicKeyVerify>>>
CreateKeyManager(const std::string& type_url) {
  if (type_url == EcdsaVerifyKeyManager::kKeyType) {
    std::unique_ptr<KeyManager<PublicKeyVerify>> manager(
        new EcdsaVerifyKeyManager());
    return std::move(manager);
  }
  return ToStatusF(crypto::tink::util::error::NOT_FOUND,
                   "No key manager for type_url '%s'.", type_url.c_str());
}

}  // anonymous namespace

crypto::tink::util::StatusOr<std::unique_ptr<KeyManager<PublicKeyVerify>>>
PublicKeyVerifyCatalogue::GetKeyManager(const std::string& type_url,
                                      const std::string& primitive_name,
                                      uint32_t min_version) const {
  if (!(absl::AsciiStrToLower(primitive_name) == "publickeyverify")) {
    return ToStatusF(crypto::tink::util::error::NOT_FOUND,
                     "This catalogue does not support primitive %s.",
                     primitive_name.c_str());
  }
  auto manager_result = CreateKeyManager(type_url);
  if (!manager_result.ok()) return manager_result;
  if (manager_result.ValueOrDie()->get_version() < min_version) {
    return ToStatusF(
        crypto::tink::util::error::NOT_FOUND,
        "No key manager for type_url '%s' with version at least %d.",
        type_url.c_str(), min_version);
  }
  return std::move(manager_result.ValueOrDie());
}

}  // namespace tink
}  // namespace crypto
