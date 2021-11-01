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

#ifndef DATA_STORAGE_RDB_SMS_MMS_HELPER_H
#define DATA_STORAGE_RDB_SMS_MMS_HELPER_H

#include "rdb_base_helper.h"
#include "sms_mms_data.h"

namespace OHOS {
namespace Telephony {
class RdbSmsMmsHelper : public RdbBaseHelper {
public:
    RdbSmsMmsHelper();
    ~RdbSmsMmsHelper() = default;

    int32_t DeleteDataByThirty();
    int32_t InsertSmsMmsInfo(int64_t &id, const NativeRdb::ValuesBucket &initialValues);
    int32_t BatchInsertSmsMmsInfo(int64_t &id, const std::vector<NativeRdb::ValuesBucket> &values);
    std::unique_ptr<NativeRdb::AbsSharedResultSet> StatisticsUnRead();
    std::unique_ptr<NativeRdb::AbsSharedResultSet> QueryMaxGroupId();
    void UpdateDbPath(const std::string &path);
    void Init();

private:
    void CreateSmsMmsInfoTableStr(std::string &createTableStr);
    void CreateSmsSubsectionTableStr(std::string &createTableStr);
    void CreateMmsProtocolTableStr(std::string &createTableStr);
    void CreateMmsPartTableStr(std::string &createTableStr);

private:
    const std::string DB_NAME = "sms_mms.db";
    std::string dbPath_ = FOLDER_PATH + DB_NAME;
    int errCode_ = NativeRdb::E_OK;
    int version_ = 1;
};
} // namespace Telephony
} // namespace OHOS
#endif // DATA_STORAGE_RDB_SMS_MMS_HELPER_H
