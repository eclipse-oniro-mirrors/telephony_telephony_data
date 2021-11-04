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
using CmdProcessFunc = int (*)(std::shared_ptr<AppExecFwk::DataAbilityHelper> helper);
std::map<char, CmdProcessFunc> g_simFuncMap;
std::map<char, CmdProcessFunc> g_smsFuncMap;
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
    value.PutString(SimData::PHONE_NUMBER, "134xxxxxxxx");
    return helper->Insert(uri, value);
}

int SimUpdate(std::shared_ptr<AppExecFwk::DataAbilityHelper> helper)
{
    Uri uri("dataability:///com.ohos.simability/sim/sim_info");
    std::string slot = std::to_string(1);
    NativeRdb::ValuesBucket values;
    values.PutString(SimData::SHOW_NAME, "China Mobile");
    NativeRdb::DataAbilityPredicates predicates;
    predicates.EqualTo(SimData::SLOT_INDEX, slot);
    return helper->Update(uri, values, predicates);
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

int SimDelete(std::shared_ptr<AppExecFwk::DataAbilityHelper> helper)
{
    Uri uri("dataability:///com.ohos.simability/sim/sim_info");
    NativeRdb::DataAbilityPredicates predicates;
    predicates.EqualTo(SimData::SLOT_INDEX, "1");
    return helper->Delete(uri, predicates);
}

int SmsInsert(std::shared_ptr<AppExecFwk::DataAbilityHelper> helper)
{
    Uri uri("dataability:///com.ohos.smsmmsability/sms_mms/sms_mms_info");
    NativeRdb::ValuesBucket value;
    value.PutString(SmsMmsInfo::RECEIVER_NUMBER, "134xxxxxxxx");
    value.PutString(SmsMmsInfo::MSG_CONTENT, "The first test text message content");
    value.PutInt(SmsMmsInfo::GROUP_ID, 1);
    return helper->Insert(uri, value);
}

int SmsUpdate(std::shared_ptr<AppExecFwk::DataAbilityHelper> helper)
{
    Uri uri("dataability:///com.ohos.smsmmsability/sms_mms/sms_mms_info");
    NativeRdb::ValuesBucket values;
    values.PutString(SmsMmsInfo::MSG_CONTENT, "The second test text message content");
    NativeRdb::DataAbilityPredicates predicates;
    predicates.EqualTo(SmsMmsInfo::MSG_ID, "1");
    return helper->Update(uri, values, predicates);
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

int SmsDelete(std::shared_ptr<AppExecFwk::DataAbilityHelper> helper)
{
    Uri uri("dataability:///com.ohos.smsmmsability/sms_mms/sms_mms_info");
    NativeRdb::DataAbilityPredicates predicates;
    predicates.EqualTo(SmsMmsInfo::MSG_ID, "1");
    return helper->Delete(uri, predicates);
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

void Init()
{
    g_simFuncMap['q'] = SimInsert;
    g_simFuncMap['w'] = SimUpdate;
    g_simFuncMap['e'] = SimSelect;
    g_simFuncMap['r'] = SimDelete;
    g_smsFuncMap['t'] = SmsInsert;
    g_smsFuncMap['y'] = SmsUpdate;
    g_smsFuncMap['u'] = SmsSelect;
    g_smsFuncMap['i'] = SmsDelete;
    g_smsFuncMap['o'] = PdpSelect;
}

int VerifyCmd(char inputCMD, std::shared_ptr<AppExecFwk::DataAbilityHelper> &helper)
{
    auto itFunSim = g_simFuncMap.find(inputCMD);
    if (itFunSim != g_simFuncMap.end()) {
        auto memberFunc = itFunSim->second;
        if (memberFunc != nullptr) {
            helper = CreateSimHelper();
            (*memberFunc)(helper);
            return 0;
        }
    }
    auto itFunSms = g_smsFuncMap.find(inputCMD);
    if (itFunSms != g_smsFuncMap.end()) {
        auto memberFunc = itFunSms->second;
        if (memberFunc != nullptr) {
            helper = CreateSmsHelper();
            (*memberFunc)(helper);
            return 0;
        }
    }
    return -1;
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

void Looper()
{
    char inputCMD = '0';
    bool loopFlag = true;
    std::shared_ptr<AppExecFwk::DataAbilityHelper> helper = nullptr;
    Init();
    while (loopFlag) {
        PrintfHint();
        std::cin >> inputCMD;
        int ret = VerifyCmd(inputCMD, helper);
        if (ret == 0) {
            return;
        }
        switch (inputCMD) {
            case 'o': {
                helper = CreateSmsHelper();
                if (helper != nullptr) {
                    PdpSelect(helper);
                }
                break;
            }
            case 'z': {
                loopFlag = false;
                break;
            }
            default: {
                break;
            }
        }
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
