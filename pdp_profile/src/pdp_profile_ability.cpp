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

#include "pdp_profile_ability.h"

#include "abs_shared_result_set.h"
#include "data_ability_predicates.h"
#include "values_bucket.h"
#include "predicates_utils.h"

#include "data_storage_errors.h"

namespace OHOS {
using AppExecFwk::AbilityLoader;
using AppExecFwk::Ability;
namespace Telephony {
std::map<std::string, PdpProfileUriType> pdpProfileUriMap = {
    {"/net/pdp_profile", PdpProfileUriType::PDP_PROFILE},
    {"/net/pdp_profile/reset", PdpProfileUriType::RESET}
};

void PdpProfileAbility::OnStart(const AppExecFwk::Want &want)
{
    DATA_STORAGE_LOGD("PdpProfileAbility::OnStart\n");
    Ability::OnStart(want);
    std::string path = GetDatabaseDir();
    if (!path.empty()) {
        path.append("/");
        helper_.UpdateDbPath(path);
    }
    helper_.Init();
    DATA_STORAGE_LOGD("PdpProfileAbility::OnStart ends##uri = %{public}s\n", path.c_str());
}

int PdpProfileAbility::Insert(const Uri &uri, const NativeRdb::ValuesBucket &value)
{
    std::lock_guard<std::mutex> guard(lock_);
    DATA_STORAGE_LOGD("PdpProfileAbility::Insert##uri = %{public}s\n", uri.ToString().c_str());
    Uri tempUri = uri;
    PdpProfileUriType pdpProfileUriType = ParseUriType(tempUri);
    int64_t id = TELEPHONY_ERROR;
    switch (pdpProfileUriType) {
        case PdpProfileUriType::PDP_PROFILE: {
            helper_.Insert(id, value, TABLE_PDP_PROFILE);
            DATA_STORAGE_LOGD("PdpProfileAbility::Insert SMS_MMS##id = %{public}" PRId64 "\n", id);
            break;
        }
        default:
            break;
    }
    DATA_STORAGE_LOGD("PdpProfileAbility::Insert end##id = %{public}" PRId64 "\n", id);
    return id;
}

std::shared_ptr<NativeRdb::AbsSharedResultSet> PdpProfileAbility::Query(
    const Uri &uri, const std::vector<std::string> &columns, const NativeRdb::DataAbilityPredicates &predicates)
{
    DATA_STORAGE_LOGD("PdpProfileAbility::Query##uri = %{public}s\n", uri.ToString().c_str());
    Uri tempUri = uri;
    PdpProfileUriType pdpProfileUriType = ParseUriType(tempUri);
    NativeRdb::AbsRdbPredicates *absRdbPredicates = nullptr;
    std::unique_ptr<NativeRdb::AbsSharedResultSet> resultSet;
    switch (pdpProfileUriType) {
        case PdpProfileUriType::PDP_PROFILE: {
            absRdbPredicates = new NativeRdb::AbsRdbPredicates(TABLE_PDP_PROFILE);
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
        DATA_STORAGE_LOGD("PdpProfileAbility::Query ------ ");
    }
    return resultSet;
}

int PdpProfileAbility::Update(
    const Uri &uri, const NativeRdb::ValuesBucket &value, const NativeRdb::DataAbilityPredicates &predicates)
{
    std::lock_guard<std::mutex> guard(lock_);
    DATA_STORAGE_LOGD("PdpProfileAbility::Update##uri = %{public}s\n", uri.ToString().c_str());
    Uri tempUri = uri;
    PdpProfileUriType pdpProfileUriType = ParseUriType(tempUri);
    int result = TELEPHONY_ERROR;
    NativeRdb::AbsRdbPredicates *absRdbPredicates = nullptr;
    switch (pdpProfileUriType) {
        case PdpProfileUriType::PDP_PROFILE: {
            absRdbPredicates = new NativeRdb::AbsRdbPredicates(TABLE_PDP_PROFILE);
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
        DATA_STORAGE_LOGD("PdpProfileAbility::Update##result = %{public}d, changedRows = %{public}d\n", result,
                          changedRows);
    }
    DATA_STORAGE_LOGD("PdpProfileAbility::Update end##result = %{public}d\n", result);
    return result;
}

int PdpProfileAbility::Delete(const Uri &uri, const NativeRdb::DataAbilityPredicates &predicates)
{
    std::lock_guard<std::mutex> guard(lock_);
    DATA_STORAGE_LOGD("PdpProfileAbility::Delete##uri = %{public}s\n", uri.ToString().c_str());
    Uri tempUri = uri;
    PdpProfileUriType pdpProfileUriType = ParseUriType(tempUri);
    int result = TELEPHONY_ERROR;
    NativeRdb::AbsRdbPredicates *absRdbPredicates = nullptr;
    switch (pdpProfileUriType) {
        case PdpProfileUriType::PDP_PROFILE: {
            absRdbPredicates = new NativeRdb::AbsRdbPredicates(TABLE_PDP_PROFILE);
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
        DATA_STORAGE_LOGD("PdpProfileAbility::Delete##result = %{public}d, deletedRows = %{public}d\n", result,
                          deletedRows);
        free(absRdbPredicates);
    }
    DATA_STORAGE_LOGD("PdpProfileAbility::Delete end##result = %{public}d\n", result);
    return result;
}

std::string PdpProfileAbility::GetType(const Uri &uri)
{
    DATA_STORAGE_LOGD("PdpProfileAbility::GetType##uri = %{public}s\n", uri.ToString().c_str());
    std::string retval(uri.ToString());
    return retval;
}

int PdpProfileAbility::OpenFile(const Uri &uri, const std::string &mode)
{
    DATA_STORAGE_LOGD("PdpProfileAbility::OpenFile##uri = %{public}s\n", uri.ToString().c_str());
    Uri tempUri = uri;
    PdpProfileUriType pdpProfileUriType = ParseUriType(tempUri);
    return static_cast<int>(pdpProfileUriType);
}

PdpProfileUriType PdpProfileAbility::ParseUriType(Uri &uri)
{
    DATA_STORAGE_LOGD("PdpProfileAbility::ParseUriType start\n");
    PdpProfileUriType pdpProfileUriType = PdpProfileUriType::UNKNOW;
    std::string uriPath = uri.ToString();
    if (!uriPath.empty()) {
        helper_.ReplaceAllStr(uriPath, ":///", "://");
        Uri tempUri(uriPath);
        std::string path = tempUri.GetPath();
        if (!path.empty()) {
            DATA_STORAGE_LOGD("PdpProfileAbility::ParseUriType##path = %{public}s\n", path.c_str());
            std::map<std::string, PdpProfileUriType>::iterator it = pdpProfileUriMap.find(path);
            if (it != pdpProfileUriMap.end()) {
                pdpProfileUriType = it->second;
                DATA_STORAGE_LOGD("PdpProfileAbility::ParseUriType##pdpProfileUriType = %{public}d\n",
                                  pdpProfileUriType);
            }
        }
    }
    return pdpProfileUriType;
}

void PdpProfileAbility::PrintfAbsRdbPredicates(const NativeRdb::AbsRdbPredicates *predicates)
{
    std::string whereClause = predicates->GetWhereClause();
    DATA_STORAGE_LOGD("PdpProfileAbility::PrintfAbsRdbPredicates##whereClause = %{public}s\n", whereClause.c_str());
    std::vector<std::string> whereArgs = predicates->GetWhereArgs();
    int32_t size = whereArgs.size();
    for (int i = 0; i < size; ++i) {
        DATA_STORAGE_LOGD("PdpProfileAbility::PrintfAbsRdbPredicates##index = %{public}d, whereArgs = %{public}s\n",
                          i, whereArgs[i].c_str());
    }
    std::string order = predicates->GetOrder();
    DATA_STORAGE_LOGD("PdpProfileAbility::PrintfAbsRdbPredicates##order = %{public}s\n", order.c_str());
    int limit = predicates->GetLimit();
    DATA_STORAGE_LOGD("PdpProfileAbility::PrintfAbsRdbPredicates##limit = %{public}d\n", limit);
    int offset = predicates->GetOffset();
    DATA_STORAGE_LOGD("PdpProfileAbility::PrintfAbsRdbPredicates##offset = %{public}d\n", offset);
    bool isDistinct = predicates->IsDistinct();
    DATA_STORAGE_LOGD("PdpProfileAbility::PrintfAbsRdbPredicates##isDistinct = %{public}d\n", isDistinct);
    std::string group = predicates->GetGroup();
    DATA_STORAGE_LOGD("PdpProfileAbility::PrintfAbsRdbPredicates##Group = %{public}s\n", group.c_str());
    std::string index = predicates->GetIndex();
    DATA_STORAGE_LOGD("PdpProfileAbility::PrintfAbsRdbPredicates##Index = %{public}s\n", index.c_str());
    bool isNeedAnd = predicates->IsNeedAnd();
    DATA_STORAGE_LOGD("PdpProfileAbility::PrintfAbsRdbPredicates##isNeedAnd = %{public}d\n", isNeedAnd);
    bool isSorted = predicates->IsSorted();
    DATA_STORAGE_LOGD("PdpProfileAbility::PrintfAbsRdbPredicates##isSorted = %{public}d\n", isSorted);
}

void PdpProfileAbility::DataAbilityPredicatesConvertAbsRdbPredicates(
    const NativeRdb::DataAbilityPredicates &dataPredicates, NativeRdb::AbsRdbPredicates *absRdbPredicates)
{
    NativeRdb::PredicatesUtils::SetWhereClauseAndArgs(
        absRdbPredicates, dataPredicates.GetWhereClause(), dataPredicates.GetWhereArgs());
    NativeRdb::PredicatesUtils::SetAttributes(absRdbPredicates, dataPredicates.IsDistinct(),
        dataPredicates.GetIndex(), dataPredicates.GetGroup(), dataPredicates.GetOrder(), dataPredicates.GetLimit(),
        dataPredicates.GetOffset());
}

REGISTER_AA(PdpProfileAbility);
}  // namespace AppExecFwk
}  // namespace OHOS
