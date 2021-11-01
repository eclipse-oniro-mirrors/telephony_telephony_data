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

#include "sim_ability.h"

#include "abs_shared_result_set.h"
#include "data_ability_predicates.h"
#include "values_bucket.h"
#include "predicates_utils.h"

#include "data_storage_errors.h"

namespace OHOS {
using AppExecFwk::AbilityLoader;
using AppExecFwk::Ability;
namespace Telephony {
std::map<std::string, SimUriType> simUriMap = {
    {"/sim/sim_info", SimUriType::SIM_INFO},
    {"/sim/sim_info/set_card", SimUriType::SET_CARD}
};

void SimAbility::OnStart(const AppExecFwk::Want &want)
{
    DATA_STORAGE_LOGD("SimAbility::OnStart\n");
    Ability::OnStart(want);
    std::string path = GetDatabaseDir();
    if (!path.empty()) {
        path.append("/");
        helper_.UpdateDbPath(path);
    }
    helper_.Init();
    DATA_STORAGE_LOGD("SimAbility::OnStart ends##uri = %{public}s\n", path.c_str());
}

int SimAbility::Insert(const Uri &uri, const NativeRdb::ValuesBucket &value)
{
    std::lock_guard<std::mutex> guard(lock_);
    DATA_STORAGE_LOGD("SimAbility::Insert##uri = %{public}s\n", uri.ToString().c_str());
    Uri tempUri = uri;
    SimUriType simUriType = ParseUriType(tempUri);
    int64_t id = TELEPHONY_ERROR;
    switch (simUriType) {
        case SimUriType::SIM_INFO: {
            helper_.Insert(id, value, TABLE_SIM_INFO);
            DATA_STORAGE_LOGD("SimAbility::Insert SMS_MMS##id = %{public}" PRId64 "\n", id);
            break;
        }
        default:
            break;
    }
    DATA_STORAGE_LOGD("SimAbility::Insert end##id = %{public}" PRId64 "\n", id);
    return id;
}

std::shared_ptr<NativeRdb::AbsSharedResultSet> SimAbility::Query(
    const Uri &uri, const std::vector<std::string> &columns, const NativeRdb::DataAbilityPredicates &predicates)
{
    DATA_STORAGE_LOGD("SimAbility::Query##uri = %{public}s\n", uri.ToString().c_str());
    Uri tempUri = uri;
    SimUriType simUriType = ParseUriType(tempUri);
    NativeRdb::AbsRdbPredicates *absRdbPredicates = nullptr;
    std::unique_ptr<NativeRdb::AbsSharedResultSet> resultSet;
    switch (simUriType) {
        case SimUriType::SIM_INFO: {
            absRdbPredicates = new NativeRdb::AbsRdbPredicates(TABLE_SIM_INFO);
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
        DATA_STORAGE_LOGD("SimAbility::Query ------ ");
    }
    return resultSet;
}

int SimAbility::Update(
    const Uri &uri, const NativeRdb::ValuesBucket &value, const NativeRdb::DataAbilityPredicates &predicates)
{
    std::lock_guard<std::mutex> guard(lock_);
    DATA_STORAGE_LOGD("SimAbility::Update##uri = %{public}s\n", uri.ToString().c_str());
    Uri tempUri = uri;
    SimUriType simUriType = ParseUriType(tempUri);
    int result = TELEPHONY_ERROR;
    NativeRdb::AbsRdbPredicates *absRdbPredicates = nullptr;
    switch (simUriType) {
        case SimUriType::SIM_INFO: {
            absRdbPredicates = new NativeRdb::AbsRdbPredicates(TABLE_SIM_INFO);
            break;
        }
        case SimUriType::SET_CARD: {
            if (!value.HasColumn(SimData::SIM_ID) || !value.HasColumn("cardType")) {
                break;
            }
            NativeRdb::ValueObject valueObject;
            bool isExistSimId = value.GetObject(SimData::SIM_ID, valueObject);
            if (!isExistSimId) {
                break;
            }
            int simId;
            result = valueObject.GetInt(simId);
            bool isExistCardType = value.GetObject("cardType", valueObject);
            if (!isExistCardType) {
                break;
            }
            int cardType;
            result = valueObject.GetInt(cardType);
            result = helper_.SetDefaultCardByType(simId, cardType);
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
        DATA_STORAGE_LOGD("SimAbility::Update##result = %{public}d, changedRows = %{public}d\n", result, changedRows);
    }
    DATA_STORAGE_LOGD("SimAbility::Update end##result = %{public}d\n", result);
    return result;
}

int SimAbility::Delete(const Uri &uri, const NativeRdb::DataAbilityPredicates &predicates)
{
    std::lock_guard<std::mutex> guard(lock_);
    DATA_STORAGE_LOGD("SimAbility::Delete##uri = %{public}s\n", uri.ToString().c_str());
    Uri tempUri = uri;
    SimUriType simUriType = ParseUriType(tempUri);
    int result = TELEPHONY_ERROR;
    NativeRdb::AbsRdbPredicates *absRdbPredicates = nullptr;
    switch (simUriType) {
        case SimUriType::SIM_INFO: {
            absRdbPredicates = new NativeRdb::AbsRdbPredicates(TABLE_SIM_INFO);
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
        DATA_STORAGE_LOGD("SimAbility::Delete##result = %{public}d, deletedRows = %{public}d\n", result, deletedRows);
        free(absRdbPredicates);
    }
    DATA_STORAGE_LOGD("SimAbility::Delete end##result = %{public}d\n", result);
    return result;
}

std::string SimAbility::GetType(const Uri &uri)
{
    DATA_STORAGE_LOGD("SimAbility::GetType##uri = %{public}s\n", uri.ToString().c_str());
    std::string retval(uri.ToString());
    return retval;
}

int SimAbility::OpenFile(const Uri &uri, const std::string &mode)
{
    DATA_STORAGE_LOGD("SimAbility::OpenFile##uri = %{public}s\n", uri.ToString().c_str());
    Uri tempUri = uri;
    SimUriType simUriType = ParseUriType(tempUri);
    return static_cast<int>(simUriType);
}

SimUriType SimAbility::ParseUriType(Uri &uri)
{
    DATA_STORAGE_LOGD("SimAbility::ParseUriType start\n");
    SimUriType simUriType = SimUriType::UNKNOW;
    std::string uriPath = uri.ToString();
    if (!uriPath.empty()) {
        helper_.ReplaceAllStr(uriPath, ":///", "://");
        Uri tempUri(uriPath);
        std::string path = tempUri.GetPath();
        if (!path.empty()) {
            DATA_STORAGE_LOGD("SimAbility::ParseUriType##path = %{public}s\n", path.c_str());
            std::map<std::string, SimUriType>::iterator it = simUriMap.find(path);
            if (it != simUriMap.end()) {
                simUriType = it->second;
                DATA_STORAGE_LOGD("SimAbility::ParseUriType##simUriType = %{public}d\n", simUriType);
            }
        }
    }
    return simUriType;
}

void SimAbility::PrintfAbsRdbPredicates(const NativeRdb::AbsRdbPredicates *predicates)
{
    std::string whereClause = predicates->GetWhereClause();
    DATA_STORAGE_LOGD("SimAbility::PrintfAbsRdbPredicates##whereClause = %{public}s\n", whereClause.c_str());
    std::vector<std::string> whereArgs = predicates->GetWhereArgs();
    int32_t size = whereArgs.size();
    for (int i = 0; i < size; ++i) {
        DATA_STORAGE_LOGD("SimAbility::PrintfAbsRdbPredicates##index = %{public}d, whereArgs = %{public}s\n", i,
                          whereArgs[i].c_str());
    }
    std::string order = predicates->GetOrder();
    DATA_STORAGE_LOGD("SimAbility::PrintfAbsRdbPredicates##order = %{public}s\n", order.c_str());
    int limit = predicates->GetLimit();
    DATA_STORAGE_LOGD("SimAbility::PrintfAbsRdbPredicates##limit = %{public}d\n", limit);
    int offset = predicates->GetOffset();
    DATA_STORAGE_LOGD("SimAbility::PrintfAbsRdbPredicates##offset = %{public}d\n", offset);
    bool isDistinct = predicates->IsDistinct();
    DATA_STORAGE_LOGD("SimAbility::PrintfAbsRdbPredicates##isDistinct = %{public}d\n", isDistinct);
    std::string group = predicates->GetGroup();
    DATA_STORAGE_LOGD("SimAbility::PrintfAbsRdbPredicates##Group = %{public}s\n", group.c_str());
    std::string index = predicates->GetIndex();
    DATA_STORAGE_LOGD("SimAbility::PrintfAbsRdbPredicates##Index = %{public}s\n", index.c_str());
    bool isNeedAnd = predicates->IsNeedAnd();
    DATA_STORAGE_LOGD("SimAbility::PrintfAbsRdbPredicates##isNeedAnd = %{public}d\n", isNeedAnd);
    bool isSorted = predicates->IsSorted();
    DATA_STORAGE_LOGD("SimAbility::PrintfAbsRdbPredicates##isSorted = %{public}d\n", isSorted);
}

void SimAbility::DataAbilityPredicatesConvertAbsRdbPredicates(
    const NativeRdb::DataAbilityPredicates &dataPredicates, NativeRdb::AbsRdbPredicates *absRdbPredicates)
{
    NativeRdb::PredicatesUtils::SetWhereClauseAndArgs(
        absRdbPredicates, dataPredicates.GetWhereClause(), dataPredicates.GetWhereArgs());
    NativeRdb::PredicatesUtils::SetAttributes(absRdbPredicates, dataPredicates.IsDistinct(),
        dataPredicates.GetIndex(), dataPredicates.GetGroup(), dataPredicates.GetOrder(), dataPredicates.GetLimit(),
        dataPredicates.GetOffset());
}

REGISTER_AA(SimAbility);
}  // namespace AppExecFwk
}  // namespace OHOS
