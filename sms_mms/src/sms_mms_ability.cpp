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

#include "sms_mms_ability.h"

#include "abs_shared_result_set.h"
#include "data_ability_predicates.h"
#include "values_bucket.h"
#include "predicates_utils.h"

#include "data_storage_errors.h"

namespace OHOS {
using AppExecFwk::Ability;
using AppExecFwk::AbilityLoader;
namespace Telephony {
std::map<std::string, MessageUriType> smsMmsUriMap = {
    {"/sms_mms/sms_mms_info", MessageUriType::SMS_MMS},
    {"/sms_mms/sms_mms_info/thirty", MessageUriType::THIRTY},
    {"/sms_mms/sms_mms_info/max_group", MessageUriType::MAX_GROUP},
    {"/sms_mms/sms_mms_info/unread_total", MessageUriType::UNREAD_TOTAL},
    {"/sms_mms/mms_protocol", MessageUriType::MMS_PROTOCOL},
    {"/sms_mms/sms_subsection", MessageUriType::SMS_SUBSECTION},
    {"/sms_mms/mms_part", MessageUriType::MMS_PART}
};

void SmsMmsAbility::OnStart(const AppExecFwk::Want &want)
{
    DATA_STORAGE_LOGD("SmsMmsAbility::OnStart\n");
    Ability::OnStart(want);
    std::string path = GetDatabaseDir();
    if (!path.empty()) {
        path.append("/");
        helper_.UpdateDbPath(path);
    }
    helper_.Init();
    DATA_STORAGE_LOGD("SmsMmsAbility::OnStart ends##uri = %{public}s\n", path.c_str());
}

int SmsMmsAbility::Insert(const Uri &uri, const NativeRdb::ValuesBucket &value)
{
    std::lock_guard<std::mutex> guard(lock_);
    DATA_STORAGE_LOGD("SmsMmsAbility::Insert##uri = %{public}s\n", uri.ToString().c_str());
    Uri tempUri = uri;
    MessageUriType messageUriType = ParseUriType(tempUri);
    int64_t id = TELEPHONY_ERROR;
    switch (messageUriType) {
        case MessageUriType::SMS_MMS: {
            helper_.Insert(id, value, TABLE_SMS_MMS_INFO);
            break;
        }
        case MessageUriType::MMS_PROTOCOL: {
            helper_.Insert(id, value, TABLE_MMS_PROTOCOL);
            break;
        }
        case MessageUriType::SMS_SUBSECTION: {
            helper_.Insert(id, value, TABLE_SMS_SUBSECTION);
            break;
        }
        case MessageUriType::MMS_PART: {
            helper_.Insert(id, value, TABLE_MMS_PART);
            break;
        }
        default:
            break;
    }
    DATA_STORAGE_LOGD("SmsMmsAbility::Insert end##id = %{public}" PRId64 "\n", id);
    return id;
}

std::shared_ptr<NativeRdb::AbsSharedResultSet> SmsMmsAbility::Query(
    const Uri &uri, const std::vector<std::string> &columns, const NativeRdb::DataAbilityPredicates &predicates)
{
    DATA_STORAGE_LOGD("SmsMmsAbility::Query##uri = %{public}s\n", uri.ToString().c_str());
    Uri tempUri = uri;
    MessageUriType messageUriType = ParseUriType(tempUri);
    NativeRdb::AbsRdbPredicates *absRdbPredicates = nullptr;
    std::unique_ptr<NativeRdb::AbsSharedResultSet> resultSet;
    switch (messageUriType) {
        case MessageUriType::SMS_MMS: {
            absRdbPredicates = new NativeRdb::AbsRdbPredicates(TABLE_SMS_MMS_INFO);
            break;
        }
        case MessageUriType::MMS_PROTOCOL: {
            absRdbPredicates = new NativeRdb::AbsRdbPredicates(TABLE_MMS_PROTOCOL);
            break;
        }
        case MessageUriType::SMS_SUBSECTION: {
            absRdbPredicates = new NativeRdb::AbsRdbPredicates(TABLE_SMS_SUBSECTION);
            break;
        }
        case MessageUriType::MMS_PART: {
            absRdbPredicates = new NativeRdb::AbsRdbPredicates(TABLE_MMS_PART);
            break;
        }
        case MessageUriType::MAX_GROUP: {
            resultSet = helper_.QueryMaxGroupId();
            break;
        }
        case MessageUriType::UNREAD_TOTAL: {
            resultSet = helper_.StatisticsUnRead();
            break;
        }
        default:
            break;
    }
    if (absRdbPredicates != nullptr) {
        DataAbilityPredicatesConvertAbsRdbPredicates(predicates, absRdbPredicates);
        PrintfAbsRdbPredicates(absRdbPredicates);
        resultSet = helper_.Query(*absRdbPredicates, columns);
        free(absRdbPredicates);
        DATA_STORAGE_LOGD("SmsMmsAbility::Query ------ ");
    }
    return resultSet;
}

int SmsMmsAbility::Update(
    const Uri &uri, const NativeRdb::ValuesBucket &value, const NativeRdb::DataAbilityPredicates &predicates)
{
    std::lock_guard<std::mutex> guard(lock_);
    DATA_STORAGE_LOGD("SmsMmsAbility::Update##uri = %{public}s\n", uri.ToString().c_str());
    Uri tempUri = uri;
    MessageUriType messageUriType = ParseUriType(tempUri);
    int result = TELEPHONY_ERROR;
    NativeRdb::AbsRdbPredicates *absRdbPredicates = nullptr;
    switch (messageUriType) {
        case MessageUriType::SMS_MMS: {
            absRdbPredicates = new NativeRdb::AbsRdbPredicates(TABLE_SMS_MMS_INFO);
            break;
        }
        case MessageUriType::MMS_PROTOCOL: {
            absRdbPredicates = new NativeRdb::AbsRdbPredicates(TABLE_MMS_PROTOCOL);
            break;
        }
        case MessageUriType::SMS_SUBSECTION: {
            absRdbPredicates = new NativeRdb::AbsRdbPredicates(TABLE_SMS_SUBSECTION);
            break;
        }
        case MessageUriType::MMS_PART: {
            absRdbPredicates = new NativeRdb::AbsRdbPredicates(TABLE_MMS_PART);
            break;
        }
        default:
            break;
    }
    if (absRdbPredicates != nullptr) {
        int changedRows;
        DataAbilityPredicatesConvertAbsRdbPredicates(predicates, absRdbPredicates);
        PrintfAbsRdbPredicates(absRdbPredicates);
        result = helper_.Update(changedRows, value, *absRdbPredicates);
        free(absRdbPredicates);
        DATA_STORAGE_LOGD(
            "SmsMmsAbility::Update##result = %{public}d, changedRows = %{public}d\n", result, changedRows);
    }
    DATA_STORAGE_LOGD("SmsMmsAbility::Update end##result = %{public}d\n", result);
    return result;
}

int SmsMmsAbility::Delete(const Uri &uri, const NativeRdb::DataAbilityPredicates &predicates)
{
    std::lock_guard<std::mutex> guard(lock_);
    DATA_STORAGE_LOGD("SmsMmsAbility::Delete##uri = %{public}s\n", uri.ToString().c_str());
    Uri tempUri = uri;
    MessageUriType messageUriType = ParseUriType(tempUri);
    int result = TELEPHONY_ERROR;
    NativeRdb::AbsRdbPredicates *absRdbPredicates = nullptr;
    switch (messageUriType) {
        case MessageUriType::SMS_MMS: {
            absRdbPredicates = new NativeRdb::AbsRdbPredicates(TABLE_SMS_MMS_INFO);
            break;
        }
        case MessageUriType::THIRTY: {
            result = helper_.DeleteDataByThirty();
            break;
        }
        case MessageUriType::MMS_PROTOCOL: {
            absRdbPredicates = new NativeRdb::AbsRdbPredicates(TABLE_MMS_PROTOCOL);
            break;
        }
        case MessageUriType::SMS_SUBSECTION: {
            absRdbPredicates = new NativeRdb::AbsRdbPredicates(TABLE_SMS_SUBSECTION);
            break;
        }
        case MessageUriType::MMS_PART: {
            absRdbPredicates = new NativeRdb::AbsRdbPredicates(TABLE_MMS_PART);
            break;
        }
        default:
            break;
    }
    if (absRdbPredicates != nullptr) {
        DataAbilityPredicatesConvertAbsRdbPredicates(predicates, absRdbPredicates);
        PrintfAbsRdbPredicates(absRdbPredicates);
        int deletedRows;
        result = helper_.Delete(deletedRows, *absRdbPredicates);
        DATA_STORAGE_LOGD(
            "SmsMmsAbility::Delete##result = %{public}d, deletedRows = %{public}d\n", result, deletedRows);
        free(absRdbPredicates);
    }
    DATA_STORAGE_LOGD("SmsMmsAbility::Delete end##result = %{public}d\n", result);
    return result;
}

std::string SmsMmsAbility::GetType(const Uri &uri)
{
    DATA_STORAGE_LOGD("SmsMmsAbility::GetType##uri = %{public}s\n", uri.ToString().c_str());
    std::string retval(uri.ToString());
    return retval;
}

int SmsMmsAbility::OpenFile(const Uri &uri, const std::string &mode)
{
    DATA_STORAGE_LOGD("SmsMmsAbility::OpenFile##uri = %{public}s\n", uri.ToString().c_str());
    Uri tempUri = uri;
    MessageUriType messageUriType = ParseUriType(tempUri);
    return static_cast<int>(messageUriType);
}

int SmsMmsAbility::BatchInsert(const Uri &uri, const std::vector<NativeRdb::ValuesBucket> &values)
{
    std::lock_guard<std::mutex> guard(lock_);
    DATA_STORAGE_LOGD("SmsMmsAbility::Insert##uri = %{public}s\n", uri.ToString().c_str());
    Uri tempUri = uri;
    MessageUriType messageUriType = ParseUriType(tempUri);
    int64_t id;
    int result = TELEPHONY_ERROR;
    switch (messageUriType) {
        case MessageUriType::SMS_MMS: {
            result = helper_.BatchInsertSmsMmsInfo(id, values);
            DATA_STORAGE_LOGD(
                "SmsMmsAbility::Insert ALL##BatchInsert = %{public}d, id = %{public}" PRId64 "\n", result, id);
            break;
        }
        case MessageUriType::MMS_PROTOCOL: {
            break;
        }
        case MessageUriType::SMS_SUBSECTION: {
            break;
        }
        case MessageUriType::MMS_PART: {
            break;
        }
        default:
            break;
    }
    DATA_STORAGE_LOGD("SmsMmsAbility::Insert end ##result = %{public}d\n", result);
    return result;
}

MessageUriType SmsMmsAbility::ParseUriType(Uri &uri)
{
    DATA_STORAGE_LOGD("SmsMmsAbility::ParseUriType start\n");
    MessageUriType messageUriType = MessageUriType::UNKNOW;
    std::string uriPath = uri.ToString();
    if (!uriPath.empty()) {
        helper_.ReplaceAllStr(uriPath, ":///", "://");
        Uri tempUri(uriPath);
        std::string path = tempUri.GetPath();
        if (!path.empty()) {
            DATA_STORAGE_LOGD("SmsMmsAbility::ParseUriType##path = %{public}s\n", path.c_str());
            std::map<std::string, MessageUriType>::iterator it = smsMmsUriMap.find(path);
            if (it != smsMmsUriMap.end()) {
                messageUriType = it->second;
                DATA_STORAGE_LOGD("SmsMmsAbility::ParseUriType##messageUriType = %{public}d\n", messageUriType);
            }
        }
    }
    return messageUriType;
}

void SmsMmsAbility::PrintfAbsRdbPredicates(const NativeRdb::AbsRdbPredicates *predicates)
{
    std::string whereClause = predicates->GetWhereClause();
    DATA_STORAGE_LOGD("SmsMmsAbility::PrintfAbsRdbPredicates##whereClause = %{public}s\n", whereClause.c_str());
    std::vector<std::string> whereArgs = predicates->GetWhereArgs();
    int32_t size = whereArgs.size();
    for (int i = 0; i < size; ++i) {
        DATA_STORAGE_LOGD("SmsMmsAbility::PrintfAbsRdbPredicates##index = %{public}d, whereArgs = %{public}s\n", i,
            whereArgs[i].c_str());
    }
    std::string order = predicates->GetOrder();
    DATA_STORAGE_LOGD("SmsMmsAbility::PrintfAbsRdbPredicates##order = %{public}s\n", order.c_str());
    int limit = predicates->GetLimit();
    DATA_STORAGE_LOGD("SmsMmsAbility::PrintfAbsRdbPredicates##limit = %{public}d\n", limit);
    int offset = predicates->GetOffset();
    DATA_STORAGE_LOGD("SmsMmsAbility::PrintfAbsRdbPredicates##offset = %{public}d\n", offset);
    bool isDistinct = predicates->IsDistinct();
    DATA_STORAGE_LOGD("SmsMmsAbility::PrintfAbsRdbPredicates##isDistinct = %{public}d\n", isDistinct);
    std::string group = predicates->GetGroup();
    DATA_STORAGE_LOGD("SmsMmsAbility::PrintfAbsRdbPredicates##Group = %{public}s\n", group.c_str());
    std::string index = predicates->GetIndex();
    DATA_STORAGE_LOGD("SmsMmsAbility::PrintfAbsRdbPredicates##Index = %{public}s\n", index.c_str());
    bool isNeedAnd = predicates->IsNeedAnd();
    DATA_STORAGE_LOGD("SmsMmsAbility::PrintfAbsRdbPredicates##isNeedAnd = %{public}d\n", isNeedAnd);
    bool isSorted = predicates->IsSorted();
    DATA_STORAGE_LOGD("SmsMmsAbility::PrintfAbsRdbPredicates##isSorted = %{public}d\n", isSorted);
}

void SmsMmsAbility::DataAbilityPredicatesConvertAbsRdbPredicates(
    const NativeRdb::DataAbilityPredicates &dataPredicates, NativeRdb::AbsRdbPredicates *absRdbPredicates)
{
    NativeRdb::PredicatesUtils::SetWhereClauseAndArgs(
        absRdbPredicates, dataPredicates.GetWhereClause(), dataPredicates.GetWhereArgs());
    NativeRdb::PredicatesUtils::SetAttributes(absRdbPredicates, dataPredicates.IsDistinct(),
        dataPredicates.GetIndex(), dataPredicates.GetGroup(), dataPredicates.GetOrder(), dataPredicates.GetLimit(),
        dataPredicates.GetOffset());
}

REGISTER_AA(SmsMmsAbility);
} // namespace Telephony
} // namespace OHOS
