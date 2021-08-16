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

#ifndef DATA_STORAGE_PDP_PROFILE_ABILITY_H
#define DATA_STORAGE_PDP_PROFILE_ABILITY_H

#include "ability_loader.h"
#include "rdb_pdp_profile_helper.h"

namespace OHOS {
namespace NativeRdb {
class AbsSharedResultSet;
class DataAbilityPredicates;
class ValuesBucket;
}
namespace Telephony {
enum class PdpProfileUriType {
    UNKNOW, PDP_PROFILE, RESET
};
class PdpProfileAbility : public AppExecFwk::Ability {
public:
    virtual void OnStart(const AppExecFwk::Want &want) override;
    virtual int Insert(const Uri &uri, const NativeRdb::ValuesBucket &value) override;
    virtual std::shared_ptr<NativeRdb::AbsSharedResultSet> Query(
        const Uri &uri, const std::vector<std::string> &columns,
        const NativeRdb::DataAbilityPredicates &predicates) override;
    virtual int Update(const Uri &uri, const NativeRdb::ValuesBucket &value,
        const NativeRdb::DataAbilityPredicates &predicates) override;
    virtual int Delete(const Uri &uri, const NativeRdb::DataAbilityPredicates &predicates) override;
    virtual std::string GetType(const Uri &uri) override;
    virtual int OpenFile(const Uri &uri, const std::string &mode) override;

private:
    PdpProfileUriType ParseUriType(Uri &uri);
    void PrintfAbsRdbPredicates(const NativeRdb::AbsRdbPredicates *predicates);
    void DataAbilityPredicatesConvertAbsRdbPredicates(
        const NativeRdb::DataAbilityPredicates &predicates, NativeRdb::AbsRdbPredicates *absRdbPredicates);

private:
    RdbPdpProfileHelper helper_;
    std::mutex lock_;
};
} // namespace Telephony
} // namespace OHOS
#endif // DATA_STORAGE_PDP_PROFILE_ABILITY_H
