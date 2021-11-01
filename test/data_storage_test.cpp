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
#include "result_set.h"
#include "uri.h"
#include "sim_data.h"
#include "sms_mms_data.h"
#include "data_storage_log_wrapper.h"

#include "rdb_pdp_profile_helper.h"

namespace OHOS {
namespace Telephony {
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

int SimUpdate(std::shared_ptr<AppExecFwk::DataAbilityHelper> helper)
{
    Uri uri("dataability:///com.ohos.simability/sim/sim_info");
    std::string slot = std::to_string(1);
    NativeRdb::ValuesBucket values;
    values.PutString(SimData::SHOW_NAME, "啥也不是");
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
    value.PutString(SmsMmsInfo::RECEIVER_NUMBER, "11111111111");
    value.PutString(SmsMmsInfo::MSG_CONTENT, "ceshi");
    value.PutInt(SmsMmsInfo::GROUP_ID, 1);
    return helper->Insert(uri, value);
}

int SmsUpdate(std::shared_ptr<AppExecFwk::DataAbilityHelper> helper)
{
    Uri uri("dataability:///com.ohos.smsmmsability/sms_mms/sms_mms_info");
    NativeRdb::ValuesBucket values;
    values.PutString(SmsMmsInfo::MSG_CONTENT, "啥也不是");
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

void PrintfHint()
{
    printf(
        "\n**********Unit Test Start**********\n"
        "usage: please input a cmd num:\n"
        "1:SimInsert()\n"
        "2:SimUpdate()\n"
        "3:SimSelect()\n"
        "4:SimDelete()\n"
        "5:SmsInsert()\n"
        "6:SmsUpdate()\n"
        "7:SmsSelect()\n"
        "8:SmsDelete()\n"
        "9:PdpSelect()\n"
        "z:exit\n"
        "***********************************\n"
        "your choice: ");
}
void SimHelperTest(char interfaceNum)
{
    std::shared_ptr<AppExecFwk::DataAbilityHelper> helper = nullptr;

    switch (interfaceNum) {
        case '1': {
            helper = CreateSimHelper();
            if (helper != nullptr) {
                SimInsert(helper);
            }
            break;
        }
        case '2': {
            helper = CreateSimHelper();
            if (helper != nullptr) {
                SimUpdate(helper);
            }
            break;
        }
        case '3': {
            helper = CreateSimHelper();
            if (helper != nullptr) {
                SimSelect(helper);
            }
            break;
        }
        case '4': {
            helper = CreateSimHelper();
            if (helper != nullptr) {
                SimDelete(helper);
            }
            break;
        }
        default: {
            break;
        }
    }
}

void SmsHelperTest(char interfaceNum)
{
    std::shared_ptr<AppExecFwk::DataAbilityHelper> helper = nullptr;
    switch (interfaceNum) {
        case '5': {
            helper = CreateSmsHelper();
            if (helper != nullptr) {
                SmsInsert(helper);
            }
            break;
        }
        case '6': {
            helper = CreateSmsHelper();
            if (helper != nullptr) {
                SmsUpdate(helper);
            }
            break;
        }
        case '7': {
            helper = CreateSmsHelper();
            if (helper != nullptr) {
                SmsSelect(helper);
            }
            break;
        }
        case '8': {
            helper = CreateSmsHelper();
            if (helper != nullptr) {
                SmsDelete(helper);
            }
            break;
        }
        default: {
            break;
        }
    }
}

void Looper()
{
    char interfaceNum = '0';
    bool loopFlag = true;
    std::shared_ptr<AppExecFwk::DataAbilityHelper> helper = nullptr;
    while (loopFlag) {
        PrintfHint();
        std::cin >> interfaceNum;
        if (interfaceNum >= '1' && interfaceNum <= '4') {
            SimHelperTest(interfaceNum);
        } else if (interfaceNum >= '5' && interfaceNum <= '8') {
            SmsHelperTest(interfaceNum);
        } else if (interfaceNum == '9') {
            helper = CreateSmsHelper();
            if (helper != nullptr) {
                PdpSelect(helper);
            }
        } else if (interfaceNum == 'z') {
            loopFlag = false;
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
