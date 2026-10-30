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

#include "rdb_pdp_profile_callback.h"
#include "rdb_pdp_profile_helper.h"
#include "data_storage_errors.h"
#include "data_storage_log_wrapper.h"
#include "parser_util.h"
#include "pdp_profile_data.h"
#include "rdb_errno.h"
#include "rdb_store.h"
#include "values_bucket.h"

namespace OHOS {
namespace Telephony {
namespace {
bool HasTableColumn(NativeRdb::RdbStore &rdbStore, const std::string &table, const std::string &column,
    bool &exists)
{
    exists = false;
    auto resultSet = rdbStore.QuerySql("PRAGMA table_info(" + table + ");");
    if (resultSet == nullptr) {
        return false;
    }
    int32_t nameIndex = 0;
    if (resultSet->GetColumnIndex("name", nameIndex) != NativeRdb::E_OK) {
        resultSet->Close();
        return false;
    }
    while (resultSet->GoToNextRow() == NativeRdb::E_OK) {
        std::string name;
        if (resultSet->GetString(nameIndex, name) == NativeRdb::E_OK && name == column) {
            exists = true;
            break;
        }
    }
    resultSet->Close();
    return true;
}
int UpgradePdpProfileToV2(NativeRdb::RdbStore &rdbStore)
{
    const std::string tbl = std::string(TABLE_PDP_PROFILE);
    rdbStore.ExecuteSql("ALTER TABLE " + tbl + " ADD COLUMN " +
                        std::string(PdpProfileData::MVNO_TYPE) + " TEXT;");
    rdbStore.ExecuteSql("ALTER TABLE " + tbl + " ADD COLUMN " +
                        std::string(PdpProfileData::MVNO_MATCH_DATA) + " TEXT;");
    return NativeRdb::E_OK;
}

int UpgradePdpProfileToV3(NativeRdb::RdbStore &rdbStore)
{
    const std::string tbl = std::string(TABLE_PDP_PROFILE);
    rdbStore.ExecuteSql("ALTER TABLE " + tbl + " ADD COLUMN " +
                        std::string(PdpProfileData::EDITED_STATUS) + " INTEGER;");
    rdbStore.ExecuteSql("ALTER TABLE " + tbl + " ADD COLUMN " +
                        std::string(PdpProfileData::SERVER) + " TEXT;");
    return NativeRdb::E_OK;
}

int UpgradePdpProfileToV4(NativeRdb::RdbStore &rdbStore)
{
    const std::string tbl = std::string(TABLE_PDP_PROFILE);
#ifdef OHOS_BUILD_ENABLE_TELEPHONY_EXT
    rdbStore.ExecuteSql("DELETE FROM " + tbl);
#endif
    rdbStore.ExecuteSql("ALTER TABLE " + tbl + " ADD COLUMN " +
                        std::string(PdpProfileData::OPKEY) + " TEXT;");
    return NativeRdb::E_OK;
}

int UpgradePseBaseStationToV5(NativeRdb::RdbStore &rdbStore)
{
    const std::string tbl = std::string(TABLE_PSE_BASE_STATION);
    const std::string timeStampColumn = std::string(PseBaseStationData::TIME_STAMP);

    std::string pseCreateSql;
    RdbPdpProfileHelper::CreatePseBaseStationTableStr(pseCreateSql, tbl);
    if (rdbStore.ExecuteSql(pseCreateSql) != NativeRdb::E_OK) {
        DATA_STORAGE_LOGE("create pse table failed");
        return NativeRdb::E_ERROR;
    }

    bool hasTimeStamp = false;
    if (!HasTableColumn(rdbStore, tbl, timeStampColumn, hasTimeStamp)) {
        DATA_STORAGE_LOGE("query pse table info failed");
        return NativeRdb::E_ERROR;
    }
    if (!hasTimeStamp &&
        rdbStore.ExecuteSql("ALTER TABLE " + tbl + " ADD COLUMN " + timeStampColumn + " TEXT;") !=
        NativeRdb::E_OK) {
        DATA_STORAGE_LOGE("add pse time_stamp column failed");
        return NativeRdb::E_ERROR;
    }

    if (rdbStore.ExecuteSql("DROP TRIGGER IF EXISTS " + std::string(PseBaseStationData::PSE_TRIGGER_NAME) +
        ";") != NativeRdb::E_OK) {
        DATA_STORAGE_LOGE("drop pse trigger failed");
        return NativeRdb::E_ERROR;
    }
    std::string pseTriggerSql;
    RdbPdpProfileHelper::CreatePseBaseStationTriggerStr(pseTriggerSql, tbl);
    if (rdbStore.ExecuteSql(pseTriggerSql) != NativeRdb::E_OK) {
        DATA_STORAGE_LOGE("create pse trigger failed");
        return NativeRdb::E_ERROR;
    }
    return NativeRdb::E_OK;
}
} // namespace

int RdbPdpProfileCallback::OnUpgrade(NativeRdb::RdbStore &rdbStore, int oldVersion, int newVersion)
{
    DATA_STORAGE_LOGI("upgrade start oldVersion = %{public}d, newVersion = %{public}d ", oldVersion, newVersion);
    if (oldVersion < VERSION_2 && newVersion >= VERSION_2) {
        UpgradePdpProfileToV2(rdbStore);
        oldVersion = VERSION_2;
    }
    if (oldVersion < VERSION_3 && newVersion >= VERSION_3) {
        UpgradePdpProfileToV3(rdbStore);
        oldVersion = VERSION_3;
    }
    if (oldVersion < VERSION_4 && newVersion >= VERSION_4) {
        UpgradePdpProfileToV4(rdbStore);
        oldVersion = VERSION_4;
    }
    if (oldVersion < VERSION_5 && newVersion >= VERSION_5) {
        if (UpgradePseBaseStationToV5(rdbStore) != NativeRdb::E_OK) {
            return NativeRdb::E_ERROR;
        }
        oldVersion = VERSION_5;
    }
    if (oldVersion != newVersion) {
        DATA_STORAGE_LOGE("upgrade error oldVersion = %{public}d, newVersion = %{public}d ", oldVersion, newVersion);
        return NativeRdb::E_ERROR;
    }
    return NativeRdb::E_OK;
}

int RdbPdpProfileCallback::OnDowngrade(NativeRdb::RdbStore &rdbStore, int currentVersion, int targetVersion)
{
    DATA_STORAGE_LOGI(
        "RdbPdpProfileCallback::OnDowngrade##currentVersion = %d, "
        "targetVersion = %d\n",
        currentVersion, targetVersion);
    return NativeRdb::E_OK;
}

int RdbPdpProfileCallback::OnCreate(NativeRdb::RdbStore &rdbStore)
{
    DATA_STORAGE_LOGI("RdbPdpProfileCallback::OnCreate");
    RdbBaseCallBack::OnCreate(rdbStore);
    InitData(rdbStore, TABLE_PDP_PROFILE);
    return NativeRdb::E_OK;
}

int RdbPdpProfileCallback::OnOpen(NativeRdb::RdbStore &rdbStore)
{
    DATA_STORAGE_LOGD("RdbPdpProfileCallback::OnOpen");
    return NativeRdb::E_OK;
}

void RdbPdpProfileCallback::InitData(NativeRdb::RdbStore &rdbStore, const std::string &tableName)
{
    DATA_STORAGE_LOGD("InitData start");
    ParserUtil util;
    std::vector<PdpProfile> vec;
    int resultCode = util.ParserPdpProfileJson(vec);
    if (resultCode != DATA_STORAGE_SUCCESS) {
        DATA_STORAGE_LOGE("ParserPdpProfileJson fail");
        return;
    }
    int32_t result = rdbStore.BeginTransaction();
    if (result != NativeRdb::E_OK) {
        DATA_STORAGE_LOGE("BeginTransaction error!");
        return;
    }
    DATA_STORAGE_LOGD("InitData size = %{public}zu", vec.size());
    for (size_t i = 0; i < vec.size(); i++) {
        NativeRdb::ValuesBucket value;
        util.ParserPdpProfileToValuesBucket(value, vec[i]);
        int64_t id;
        rdbStore.Insert(id, tableName, value);
    }
    rdbStore.Commit();
    DATA_STORAGE_LOGD("InitData end");
}
} // namespace Telephony
} // namespace OHOS
