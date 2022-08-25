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

#include "rdb_sim_callback.h"

#include "data_storage_log_wrapper.h"
#include "rdb_errno.h"

namespace OHOS {
namespace NativeRdb {
class RdbStore;
}
namespace Telephony {
int RdbSimCallback::OnUpgrade(NativeRdb::RdbStore &rdbStore, int oldVersion, int newVersion)
{
    DATA_STORAGE_LOGI(
        "Data_Storage RdbSimCallback::OnUpgrade##oldVersion = %d, "
        "newVersion = %d\n",
        oldVersion, newVersion);
    return NativeRdb::E_OK;
}

int RdbSimCallback::OnDowngrade(NativeRdb::RdbStore &rdbStore, int currentVersion, int targetVersion)
{
    DATA_STORAGE_LOGI(
        "Data_Storage RdbSimCallback::OnDowngrade##currentVersion = "
        "%d, targetVersion = %d\n",
        currentVersion, targetVersion);
    return NativeRdb::E_OK;
}

int RdbSimCallback::OnOpen(NativeRdb::RdbStore &rdbStore)
{
    return NativeRdb::E_OK;
}
} // namespace Telephony
} // namespace OHOS