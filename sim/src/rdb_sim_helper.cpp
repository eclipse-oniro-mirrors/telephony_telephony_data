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

#include "rdb_sim_helper.h"

#include "rdb_sim_callback.h"
#include "data_storage_errors.h"

namespace OHOS {
namespace Telephony {
void RdbSimHelper::Init()
{
    NativeRdb::RdbStoreConfig config(dbPath_);
    config.SetJournalMode(NativeRdb::JournalMode::MODE_TRUNCATE);
    std::string simInfoStr;
    CreateSimInfoTableStr(simInfoStr);
    std::vector<std::string> createTableVec;
    createTableVec.push_back(simInfoStr);
    RdbSimCallback callback(createTableVec);
    CreateRdbStore(config, version_, callback, errCode_);
}

RdbSimHelper::RdbSimHelper()
{
}

void RdbSimHelper::UpdateDbPath(const std::string &path)
{
    dbPath_ = path + DB_NAME;
}

void RdbSimHelper::CreateSimInfoTableStr(std::string &createTableStr)
{
    createTableStr.append("CREATE TABLE IF NOT EXISTS ").append(TABLE_SIM_INFO);
    createTableStr.append("(").append(SimData::SIM_ID).append(" INTEGER PRIMARY KEY, ");
    createTableStr.append(SimData::ICC_ID).append(" TEXT DEFAULT '', ");
    createTableStr.append(SimData::CARD_ID).append(" TEXT DEFAULT '', ");
    createTableStr.append(SimData::SLOT_INDEX).append(" INTEGER DEFAULT -1, ");
    createTableStr.append(SimData::SHOW_NAME).append(" TEXT DEFAULT '', ");
    createTableStr.append(SimData::PHONE_NUMBER).append(" TEXT DEFAULT '', ");
    createTableStr.append(SimData::COUNTRY_CODE).append(" TEXT DEFAULT '', ");
    createTableStr.append(SimData::IMSI).append(" TEXT DEFAULT '', ");
    createTableStr.append(SimData::IS_MAIN_CARD).append(" INTEGER DEFAULT 0, ");
    createTableStr.append(SimData::IS_MESSAGE_CARD).append(" INTEGER DEFAULT 0, ");
    createTableStr.append(SimData::IS_CELLULAR_DATA_CARD).append(" INTEGER DEFAULT 0, ");
    createTableStr.append(SimData::IS_ACTIVE).append(" INTEGER DEFAULT 0, ");
    createTableStr.append(SimData::CARD_TYPE).append(" INTEGER DEFAULT 0, ");
    createTableStr.append(SimData::LANGUAGE).append(" TEXT DEFAULT '', ");
    createTableStr.append("UNIQUE (").append(SimData::SIM_ID).append(", ").
        append(SimData::PHONE_NUMBER).append("))");
}

int32_t RdbSimHelper::SetDefaultCardByType(int32_t simId, int32_t type)
{
    int result = BeginTransaction();
    if (result != NativeRdb::E_OK) {
        return result;
    }
    DATA_STORAGE_LOGD(
        "RdbSimHelper::SetDefaultMessageCard BeginTransaction result "
        "= %{public}d", result);
    result = UpdateCardStateByType(type, 0, 1);
    DATA_STORAGE_LOGD(
        "RdbSimHelper::SetDefaultMessageCard UpdateCardStateByType "
        "result = %{public}d", result);
    int changedRows;
    NativeRdb::ValuesBucket values;
    switch (type) {
        case MAIN: {
            values.PutInt(SimData::IS_MAIN_CARD, 1);
            break;
        }
        case MESSAGE: {
            values.PutInt(SimData::IS_MESSAGE_CARD, 1);
            break;
        }
        case CELLULAR_DATA: {
            values.PutInt(SimData::IS_CELLULAR_DATA_CARD, 1);
            break;
        }
        default:
            DATA_STORAGE_LOGE("RdbSimHelper::SetDefaultDataByType is error!");
            return TELEPHONY_ERROR;
    }
    std::string whereClause;
    whereClause.append(SimData::SIM_ID).append("=").append(std::to_string(simId));
    result = Update(changedRows, TABLE_SIM_INFO, values, whereClause);
    DATA_STORAGE_LOGD("RdbSimHelper::SetDefaultMessageCard Update result = %{public}d", result);
    result = MarkAsCommit();
    DATA_STORAGE_LOGD("RdbSimHelper::SetDefaultMessageCard MarkAsCommit result = %{public}d", result);
    result = EndTransaction();
    DATA_STORAGE_LOGD("RdbSimHelper::SetDefaultMessageCard EndTransaction result = %{public}d", result);
    return result;
}

int32_t RdbSimHelper::UpdateCardStateByType(int32_t type, int32_t updateState, int32_t whereSate)
{
    int32_t result = TELEPHONY_ERROR;
    bool isExist = false;
    NativeRdb::ValuesBucket values;
    std::string whereClause;
    switch (type) {
        case MAIN: {
            isExist = true;
            values.PutInt(SimData::IS_MAIN_CARD, updateState);
            whereClause.append(SimData::IS_MAIN_CARD).append("=").append(std::to_string(whereSate));
            break;
        }
        case MESSAGE: {
            isExist = true;
            values.PutInt(SimData::IS_MESSAGE_CARD, updateState);
            whereClause.append(SimData::IS_MESSAGE_CARD).append("=").append(std::to_string(whereSate));
            break;
        }
        case CELLULAR_DATA: {
            isExist = true;
            values.PutInt(SimData::IS_CELLULAR_DATA_CARD, updateState);
            whereClause.append(SimData::IS_CELLULAR_DATA_CARD).append("=").append(std::to_string(whereSate));
            break;
        }
        default:
            break;
    }
    if (isExist) {
        int changedRows;
        return Update(changedRows, TABLE_SIM_INFO, values, whereClause);
    }
    return result;
}

int32_t RdbSimHelper::ClearData()
{
    std::string sql;
    sql.append("delete from ").append(TABLE_SIM_INFO);
    return ExecuteSql(sql);
}
} // namespace Telephony
} // namespace OHOS