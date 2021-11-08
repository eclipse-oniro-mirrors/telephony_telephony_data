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

#include "data_storage_gtest.h"

#include "data_storage_log_wrapper.h"
#include "sim_data.h"
#include "sms_mms_data.h"

namespace OHOS {
namespace Telephony {
using namespace testing::ext;
void DataStorageGtest::SetUpTestCase(void)
{
    // step 3: Set Up Test Case
}

void DataStorageGtest::TearDownTestCase(void)
{
    // step 3: Tear Down Test Case
}

void DataStorageGtest::SetUp(void)
{
    // step 3: input testcase setup step
}

void DataStorageGtest::TearDown(void)
{
    // step 3: input testcase teardown step
}

std::shared_ptr<AppExecFwk::DataAbilityHelper> DataStorageGtest::CreateDataAHelper(int32_t systemAbilityId) const
{
    DATA_STORAGE_LOGD("DataStorageGtest::CreateDataAHelper");
    auto saManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (saManager == nullptr) {
        DATA_STORAGE_LOGE("DataStorageGtest Get system ability mgr failed.");
        return nullptr;
    }
    auto remoteObj = saManager->GetSystemAbility(systemAbilityId);
    while (remoteObj == nullptr) {
        DATA_STORAGE_LOGE("DataStorageGtest GetSystemAbility Service Failed.");
        return nullptr;
    }
    return AppExecFwk::DataAbilityHelper::Creator(remoteObj);
}

std::shared_ptr<AppExecFwk::DataAbilityHelper> DataStorageGtest::CreateSimHelper() const
{
    return CreateDataAHelper(TELEPHONY_CORE_SERVICE_SYS_ABILITY_ID);
}

std::shared_ptr<AppExecFwk::DataAbilityHelper> DataStorageGtest::CreateSmsHelper() const
{
    return CreateDataAHelper(TELEPHONY_SMS_MMS_SYS_ABILITY_ID);
}

int DataStorageGtest::SimInsert(const std::shared_ptr<AppExecFwk::DataAbilityHelper> &helper) const
{
    Uri uri("dataability:///com.ohos.simability/sim/sim_info");
    NativeRdb::ValuesBucket value;
    value.PutInt(SimData::SIM_ID, 1);
    value.PutInt(SimData::SLOT_INDEX, 1);
    value.PutString(SimData::PHONE_NUMBER, "134xxxxxxxx");
    value.PutString(SimData::ICC_ID, "icc_id");
    value.PutString(SimData::CARD_ID, "card_id");
    return helper->Insert(uri, value);
}

int DataStorageGtest::SimUpdate(const std::shared_ptr<AppExecFwk::DataAbilityHelper> &helper) const
{
    Uri uri("dataability:///com.ohos.simability/sim/sim_info");
    std::string slot = std::to_string(1);
    NativeRdb::ValuesBucket values;
    values.PutString(SimData::SHOW_NAME, "China Mobile");
    NativeRdb::DataAbilityPredicates predicates;
    predicates.EqualTo(SimData::SLOT_INDEX, slot);
    return helper->Update(uri, values, predicates);
}

int DataStorageGtest::SimSelect(const std::shared_ptr<AppExecFwk::DataAbilityHelper> &helper) const
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

int DataStorageGtest::SimDelete(const std::shared_ptr<AppExecFwk::DataAbilityHelper> &helper) const
{
    Uri uri("dataability:///com.ohos.simability/sim/sim_info");
    NativeRdb::DataAbilityPredicates predicates;
    predicates.EqualTo(SimData::SLOT_INDEX, "1");
    return helper->Delete(uri, predicates);
}

int DataStorageGtest::SmsInsert(const std::shared_ptr<AppExecFwk::DataAbilityHelper> &helper) const
{
    Uri uri("dataability:///com.ohos.smsmmsability/sms_mms/sms_mms_info");
    NativeRdb::ValuesBucket value;
    value.PutString(SmsMmsInfo::RECEIVER_NUMBER, "134xxxxxxxx");
    value.PutString(SmsMmsInfo::MSG_CONTENT, "The first test text message content");
    value.PutInt(SmsMmsInfo::GROUP_ID, 1);
    value.PutInt(SmsMmsInfo::IS_SENDER, 0);
    return helper->Insert(uri, value);
}

int DataStorageGtest::SmsUpdate(const std::shared_ptr<AppExecFwk::DataAbilityHelper> &helper) const
{
    Uri uri("dataability:///com.ohos.smsmmsability/sms_mms/sms_mms_info");
    NativeRdb::ValuesBucket values;
    values.PutString(SmsMmsInfo::MSG_CONTENT, "The second test text message content");
    NativeRdb::DataAbilityPredicates predicates;
    predicates.EqualTo(SmsMmsInfo::MSG_ID, "1");
    return helper->Update(uri, values, predicates);
}

int DataStorageGtest::SmsSelect(const std::shared_ptr<AppExecFwk::DataAbilityHelper> &helper) const
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

int DataStorageGtest::SmsDelete(const std::shared_ptr<AppExecFwk::DataAbilityHelper> &helper) const
{
    Uri uri("dataability:///com.ohos.smsmmsability/sms_mms/sms_mms_info");
    NativeRdb::DataAbilityPredicates predicates;
    predicates.EqualTo(SmsMmsInfo::MSG_ID, "1");
    return helper->Delete(uri, predicates);
}

HWTEST_F(DataStorageGtest, SimInsert_001, TestSize.Level1)
{
    std::shared_ptr<AppExecFwk::DataAbilityHelper> helper = CreateSimHelper();
    int ret = -1;
    if (helper != nullptr) {
        ret = SimInsert(helper);
    }
}

HWTEST_F(DataStorageGtest, SimUpdate_001, TestSize.Level1)
{
    std::shared_ptr<AppExecFwk::DataAbilityHelper> helper = CreateSimHelper();
    int ret = -1;
    if (helper != nullptr) {
        ret = SimUpdate(helper);
    }
}

HWTEST_F(DataStorageGtest, SimSelect_001, TestSize.Level1)
{
    std::shared_ptr<AppExecFwk::DataAbilityHelper> helper = CreateSimHelper();
    int ret = -1;
    if (helper != nullptr) {
        ret = SimSelect(helper);
    }
}

HWTEST_F(DataStorageGtest, SimDelete_001, TestSize.Level1)
{
    std::shared_ptr<AppExecFwk::DataAbilityHelper> helper = CreateSimHelper();
    int ret = -1;
    if (helper != nullptr) {
        ret = SimDelete(helper);
    }
}

HWTEST_F(DataStorageGtest, SmsInsert_001, TestSize.Level1)
{
    std::shared_ptr<AppExecFwk::DataAbilityHelper> helper = CreateSmsHelper();
    int ret = -1;
    if (helper != nullptr) {
        ret = SmsInsert(helper);
    }
}

HWTEST_F(DataStorageGtest, SmsUpdate_001, TestSize.Level1)
{
    std::shared_ptr<AppExecFwk::DataAbilityHelper> helper = CreateSmsHelper();
    int ret = -1;
    if (helper != nullptr) {
        ret = SmsUpdate(helper);
    }
}

HWTEST_F(DataStorageGtest, SmsSelect_001, TestSize.Level1)
{
    std::shared_ptr<AppExecFwk::DataAbilityHelper> helper = CreateSmsHelper();
    int ret = -1;
    if (helper != nullptr) {
        ret = SmsSelect(helper);
    }
}

HWTEST_F(DataStorageGtest, SmsDelete_001, TestSize.Level1)
{
    std::shared_ptr<AppExecFwk::DataAbilityHelper> helper = CreateSmsHelper();
    int ret = -1;
    if (helper != nullptr) {
        ret = SmsDelete(helper);
    }
}
} // namespace Telephony
} // namespace OHOS