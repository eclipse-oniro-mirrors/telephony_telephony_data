/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <iostream>
#include <vector>
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "data_ability_helper.h"
#include "data_ability_predicates.h"
#include "abs_shared_result_set.h"
#include "values_bucket.h"
#include "uri.h"
#include "sim_data.h"
#include "sms_mms_data.h"
#include "data_storage_log_wrapper.h"

namespace OHOS {
namespace Telephony {
using NetdTestFunc = void (*)();
std::map<int32_t, NetdTestFunc> g_memberFuncMap;

std::shared_ptr<AppExecFwk::DataAbilityHelper> CreateDataAHelper(int32_t systemAbilityId)
{
    DATA_STORAGE_LOGD("DataSimRdbHelper::CreateDataAHelper");
    auto saManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (saManager == nullptr) {
        DATA_STORAGE_LOGE("DataSimRdbHelper Get system ability mgr failed.");
        return nullptr;
    }
    auto remoteObj = saManager->GetSystemAbility(systemAbilityId);
    while (remoteObj == nullptr) {
        DATA_STORAGE_LOGE("DataSimRdbHelper GetSystemAbility Service Failed.");
        return nullptr;
    }
    return AppExecFwk::DataAbilityHelper::Creator(remoteObj);
}

std::shared_ptr<AppExecFwk::DataAbilityHelper> CreateSimHelper()
{
    return CreateDataAHelper(TELEPHONY_CORE_SERVICE_SYS_ABILITY_ID);
}

std::shared_ptr<AppExecFwk::DataAbilityHelper> CreateSmsHelper()
{
    return CreateDataAHelper(TELEPHONY_SMS_MMS_SYS_ABILITY_ID);
}

int SimInsert(std::shared_ptr<AppExecFwk::DataAbilityHelper> helper)
{
    Uri uri("dataability:///com.ohos.simability/sim/sim_info");
    NativeRdb::ValuesBucket value;
    value.PutInt(SimData::SIM_ID, 1);
    value.PutInt(SimData::SLOT_INDEX, 1);
    value.PutString(SimData::PHONE_NUMBER, "1111111111111");
    return helper->Insert(uri, value);
}

void SimInsertFun()
{
    std::shared_ptr<AppExecFwk::DataAbilityHelper> helper = nullptr;
    helper = CreateSimHelper();
    if (helper != nullptr) {
        SimInsert(helper);
    }
}

int SimUpdate(std::shared_ptr<AppExecFwk::DataAbilityHelper> helper)
{
    Uri uri("dataability:///com.ohos.simability/sim/sim_info");
    std::string slot = std::to_string(1);
    NativeRdb::ValuesBucket values;
    values.PutString(SimData::SHOW_NAME, "test");
    NativeRdb::DataAbilityPredicates predicates;
    predicates.EqualTo(SimData::SLOT_INDEX, slot);
    return helper->Update(uri, values, predicates);
}

void SimUpdateFun()
{
    std::shared_ptr<AppExecFwk::DataAbilityHelper> helper = nullptr;
    helper = CreateSimHelper();
    if (helper != nullptr) {
        SimUpdate(helper);
    }
}

int SimSelect(std::shared_ptr<AppExecFwk::DataAbilityHelper> helper)
{
    Uri uri("dataability:///com.ohos.simability/sim/sim_info");
    std::vector<std::string> colume;
    NativeRdb::DataAbilityPredicates predicates;
    std::shared_ptr<NativeRdb::AbsSharedResultSet> resultSet = helper->Query(uri, colume, predicates);
    if (resultSet != nullptr) {
        int count;
        resultSet->GetRowCount(count);
        return count;
    }
    return -1;
}

void SimSelectFun()
{
    std::shared_ptr<AppExecFwk::DataAbilityHelper> helper = nullptr;
    helper = CreateSimHelper();
    if (helper != nullptr) {
        SimSelect(helper);
    }
}

int SimDelete(std::shared_ptr<AppExecFwk::DataAbilityHelper> helper)
{
    Uri uri("dataability:///com.ohos.simability/sim/sim_info");
    NativeRdb::DataAbilityPredicates predicates;
    predicates.EqualTo(SimData::SLOT_INDEX, "1");
    return helper->Delete(uri, predicates);
}

void SimDeleteFun()
{
    std::shared_ptr<AppExecFwk::DataAbilityHelper> helper = nullptr;
    helper = CreateSimHelper();
    if (helper != nullptr) {
        SimDelete(helper);
    }
}

int SmsInsert(std::shared_ptr<AppExecFwk::DataAbilityHelper> helper)
{
    Uri uri("dataability:///com.ohos.smsmmsability/sms_mms/sms_mms_info");
    NativeRdb::ValuesBucket value;
    value.PutString(SmsMmsInfo::RECEIVER_NUMBER, "11111111111");
    value.PutString(SmsMmsInfo::MSG_CONTENT, "test");
    value.PutInt(SmsMmsInfo::GROUP_ID, 1);
    return helper->Insert(uri, value);
}

void SmsInsertFun()
{
    std::shared_ptr<AppExecFwk::DataAbilityHelper> helper = nullptr;
    helper = CreateSmsHelper();
    if (helper != nullptr) {
        SmsInsert(helper);
    }
}

int SmsUpdate(std::shared_ptr<AppExecFwk::DataAbilityHelper> helper)
{
    Uri uri("dataability:///com.ohos.smsmmsability/sms_mms/sms_mms_info");
    NativeRdb::ValuesBucket values;
    values.PutString(SmsMmsInfo::MSG_CONTENT, "test");
    NativeRdb::DataAbilityPredicates predicates;
    predicates.EqualTo(SmsMmsInfo::MSG_ID, "1");
    return helper->Update(uri, values, predicates);
}

void SmsUpdateFun()
{
    std::shared_ptr<AppExecFwk::DataAbilityHelper> helper = nullptr;
    helper = CreateSmsHelper();
    if (helper != nullptr) {
        SmsUpdate(helper);
    }
}

int SmsSelect(std::shared_ptr<AppExecFwk::DataAbilityHelper> helper)
{
    Uri uri("dataability:///com.ohos.smsmmsability/sms_mms/sms_mms_info");
    std::vector<std::string> colume;
    NativeRdb::DataAbilityPredicates predicates;
    std::shared_ptr<NativeRdb::AbsSharedResultSet> resultSet = helper->Query(uri, colume, predicates);
    if (resultSet != nullptr) {
        int count;
        resultSet->GetRowCount(count);
        return count;
    }
    return -1;
}

void SmsSelectFun()
{
    std::shared_ptr<AppExecFwk::DataAbilityHelper> helper = nullptr;
    helper = CreateSmsHelper();
    if (helper != nullptr) {
        SmsSelect(helper);
    }
}

int SmsDelete(std::shared_ptr<AppExecFwk::DataAbilityHelper> helper)
{
    Uri uri("dataability:///com.ohos.smsmmsability/sms_mms/sms_mms_info");
    NativeRdb::DataAbilityPredicates predicates;
    predicates.EqualTo(SmsMmsInfo::MSG_ID, "1");
    return helper->Delete(uri, predicates);
}

void SmsDeleteFun()
{
    std::shared_ptr<AppExecFwk::DataAbilityHelper> helper = nullptr;
    helper = CreateSmsHelper();
    if (helper != nullptr) {
        SmsDelete(helper);
    }
}

int PdpSelect(std::shared_ptr<AppExecFwk::DataAbilityHelper> helper)
{
    Uri uri("dataability:///com.ohos.pdpprofileability/net/pdp_profile");
    std::vector<std::string> colume;
    NativeRdb::DataAbilityPredicates predicates;
    std::shared_ptr<NativeRdb::AbsSharedResultSet> resultSet = helper->Query(uri, colume, predicates);
    if (resultSet != nullptr) {
        int count;
        resultSet->GetRowCount(count);
        return count;
    }
    return -1;
}

void PdpSelectFun()
{
    std::shared_ptr<AppExecFwk::DataAbilityHelper> helper = nullptr;
    helper = CreateSmsHelper();
    if (helper != nullptr) {
        PdpSelect(helper);
    }
}

int32_t GetInputData()
{
    int32_t input;
    std::cin >> input;
    while (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore();
        printf("Input error, please input number again\n");
        std::cin >> input;
    }
    return input;
}

void PrintfHint()
{
    printf(
        "\n**********Unit Test Start**********\n"
        "usage: please input a cmd num:\n"
        "q:SimInsert()\n"
        "w:SimUpdate()\n"
        "e:SimSelect()\n"
        "r:SimDelete()\n"
        "t:SmsInsert()\n"
        "y:SmsUpdate()\n"
        "u:SmsSelect()\n"
        "i:SmsDelete()\n"
        "o:PdpSelect()\n"
        "z:exit\n"
        "***********************************\n"
        "your choice: ");
}

void ProcessInput(bool &loopFlag)
{
    int32_t inputCMD = GetInputData();
    auto itFunc = g_memberFuncMap.find(inputCMD);
    if (itFunc != g_memberFuncMap.end()) {
        auto memberFunc = itFunc->second;
        if (memberFunc != nullptr) {
            (*memberFunc)();
            return;
        }
    }
    printf("inputCMD is:[%d]\n", inputCMD);
    switch (inputCMD) {
        case 'z': {
            loopFlag = false;
            printf("exit...\n");
            break;
        }
        default:
            printf("please input correct number...\n");
            break;
    }
}

void Init()
{
    g_memberFuncMap['q'] = SimInsertFun;
    g_memberFuncMap['w'] = SimUpdateFun;
    g_memberFuncMap['e'] = SimSelectFun;
    g_memberFuncMap['r'] = SimDeleteFun;
    g_memberFuncMap['t'] = SmsInsertFun;
    g_memberFuncMap['y'] = SmsUpdateFun;
    g_memberFuncMap['u'] = SmsSelectFun;
    g_memberFuncMap['i'] = SmsDeleteFun;
    g_memberFuncMap['o'] = PdpSelectFun;
}

void Looper()
{
    bool loopFlag = true;
    Init();

    while (loopFlag) {
        PrintfHint();
        ProcessInput(loopFlag);
    }
}
} // namespace Telephony
} // namespace OHOS

using namespace OHOS;
int main()
{
    Telephony::Looper();
    return 0;
}
