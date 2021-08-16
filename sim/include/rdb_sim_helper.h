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

#ifndef DATA_STORAGE_RDB_SIM_HELPER_H
#define DATA_STORAGE_RDB_SIM_HELPER_H

#include "rdb_base_helper.h"
#include "sim_data.h"

namespace OHOS {
namespace Telephony {
class RdbSimHelper : public RdbBaseHelper {
public:
    enum { MAIN = 0, MESSAGE, CELLULAR_DATA };

    RdbSimHelper();
    ~RdbSimHelper() = default;

    void UpdateDbPath(const std::string &path);
    int32_t ClearData();
    void Init();
    int32_t SetDefaultCardByType(int32_t simId, int32_t type);

private:
    void CreateSimInfoTableStr(std::string &createTableStr);
    int32_t UpdateCardStateByType(int32_t type, int32_t updateState, int32_t whereSate);

private:
    const std::string DB_NAME = "sim.db";
    std::string dbPath_ = FOLDER_PATH + DB_NAME;
    int errCode_ = NativeRdb::E_OK;
    int version_ = 1;
};
} // namespace Telephony
} // namespace OHOS
#endif // DATA_STORAGE_RDB_SIM_HELPER_H