/*
 * Copyright (C) 2017 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <nos/CitadeldProxyClient.h>

#include <android-base/logging.h>
#include <binder/IServiceManager.h>
#include <binder/ProcessState.h>

using ::android::defaultServiceManager;
using ::android::sp;
using ::android::IServiceManager;
using ::android::ProcessState;

using ::android::binder::Status;

using ::android::hardware::citadel::ICitadeld;

namespace nos {

CitadeldProxyClient::~CitadeldProxyClient() {
    Close();
}

void CitadeldProxyClient::Open() {
    // Ensure this process is using the vndbinder
    ProcessState::initWithDriver("/dev/vndbinder");
    _citadeld = ICitadeld::asInterface(defaultServiceManager()->getService(ICitadeld::descriptor));
}

void CitadeldProxyClient::Close() {
    _citadeld.clear();
}

bool CitadeldProxyClient::IsOpen() const {
    return _citadeld != nullptr;
}

uint32_t CitadeldProxyClient::CallApp(uint32_t appId, uint16_t arg,
                                      const std::vector<uint8_t>& request,
                                      std::vector<uint8_t>* response) {
    uint32_t appStatus;
    Status status = _citadeld->callApp(appId, arg, request, response,
                                       reinterpret_cast<int32_t*>(&appStatus));
    if (status.isOk()) {
        return appStatus;
    }
    LOG(ERROR) << "Failed to call app via citadeld: " << status.toString8();
    return 3; // TODO: Currently APP_ERROR_DRIVER
}

} // namespace nos
