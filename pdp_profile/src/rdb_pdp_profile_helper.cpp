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

#include "rdb_pdp_profile_helper.h"

#include "pdp_profile_data.h"
#include "rdb_pdp_profile_callback.h"
#include "parser_util.h"

namespace OHOS {
namespace Telephony {
void RdbPdpProfileHelper::Init()
{
    NativeRdb::RdbStoreConfig config(dbPath_);
    config.SetJournalMode(NativeRdb::JournalMode::MODE_TRUNCATE);
    std::string pdpProfileStr;
    CreatePdpProfileTableStr(pdpProfileStr, TABLE_PDP_PROFILE);
    std::vector<std::string> createTableVec;
    createTableVec.push_back(pdpProfileStr);
    RdbPdpProfileCallback callback(createTableVec);
    CreateRdbStore(config, version_, callback, errCode_);
}

RdbPdpProfileHelper::RdbPdpProfileHelper()
{
}

void RdbPdpProfileHelper::UpdateDbPath(const std::string &path)
{
    dbPath_ = path + DB_NAME;
}

void RdbPdpProfileHelper::CreatePdpProfileTableStr(std::string &createTableStr, const std::string &tableName)
{
    createTableStr.append("CREATE TABLE IF NOT EXISTS ").append(tableName).append("(");
    createTableStr.append(PdpProfileData::PROFILE_ID).append(" INTEGER PRIMARY KEY AUTOINCREMENT, ");
    createTableStr.append(PdpProfileData::PROFILE_NAME).append(" TEXT DEFAULT '', ");
    createTableStr.append(PdpProfileData::MCC).append(" TEXT DEFAULT '', ");
    createTableStr.append(PdpProfileData::MNC).append(" TEXT DEFAULT '', ");
    createTableStr.append(PdpProfileData::APN).append(" TEXT DEFAULT '', ");
    createTableStr.append(PdpProfileData::AUTH_TYPE).append(" INTEGER, ");
    createTableStr.append(PdpProfileData::AUTH_USER).append(" TEXT DEFAULT '', ");
    createTableStr.append(PdpProfileData::AUTH_PWD).append(" TEXT DEFAULT '', ");
    createTableStr.append(PdpProfileData::APN_TYPES).append(" INTEGER, ");
    createTableStr.append(PdpProfileData::IS_ROAMING_APN).append(" INTEGER, ");
    createTableStr.append(PdpProfileData::APN_PROTOCOL).append(" INTEGER, ");
    createTableStr.append(PdpProfileData::APN_ROAM_PROTOCOL).append(" INTEGER, ");
    createTableStr.append(PdpProfileData::HOME_URL).append(" TEXT DEFAULT '', ");
    createTableStr.append(PdpProfileData::MMS_IP_ADDRESS).append(" TEXT DEFAULT '', ");
    createTableStr.append(PdpProfileData::PROXY_IP_ADDRESS).append(" TEXT DEFAULT '', ");
    createTableStr.append(PdpProfileData::BEARING_SYSTEM_TYPE).append(" INTEGER, ");
    createTableStr.append("UNIQUE (").append(PdpProfileData::MCC).append(", ");
    createTableStr.append(PdpProfileData::APN).append(", ");
    createTableStr.append(PdpProfileData::APN_TYPES).append(", ");
    createTableStr.append(PdpProfileData::IS_ROAMING_APN).append(", ");
    createTableStr.append(PdpProfileData::APN_PROTOCOL).append(", ");
    createTableStr.append(PdpProfileData::APN_ROAM_PROTOCOL).append(", ");
    createTableStr.append(PdpProfileData::HOME_URL).append(", ");
    createTableStr.append(PdpProfileData::MMS_IP_ADDRESS).append(", ");
    createTableStr.append(PdpProfileData::PROXY_IP_ADDRESS).append("))");
}

void RdbPdpProfileHelper::ResetApn()
{
    int ret = BeginTransaction();
    std::string pdpProfileStr;
    CreatePdpProfileTableStr(pdpProfileStr, TEMP_TABLE_PDP_PROFILE);

    ParserUtil util;
    std::vector<PdpProfile> vec;
    int state = util.ParserPdpProfileJson(vec);
    DATA_STORAGE_LOGD("RdbPdpProfileHelper::RestoreApn state = %{public}d\n", state);
    for (size_t i = 0; i < vec.size(); i++) {
        NativeRdb::ValuesBucket value;
        util.ParserPdpProfileToValuesBucket(value, vec[i]);
        int64_t id;
        Insert(id, value, TEMP_TABLE_PDP_PROFILE);
    }

    ret = ExecuteSql("drop table " + TABLE_PDP_PROFILE);
    DATA_STORAGE_LOGD("RdbPdpProfileHelper::RestoreApn drop table ret = %{public}d\n", ret);

    std::string sql;
    sql.append("alter table ").append(TEMP_TABLE_PDP_PROFILE).append(" rename to ").append(TABLE_PDP_PROFILE);
    ret = ExecuteSql(sql);
    DATA_STORAGE_LOGD("RdbPdpProfileHelper::RestoreApn alter table ret = %{public}d\n", ret);

    ret = MarkAsCommit();
    ret = EndTransaction();
}
} // namespace Telephony
} // namespace OHOS
