/**
 * Copyright (C) 2021 Saturneric <eric@bktus.com>
 *
 * This file is part of GpgFrontend.
 *
 * GpgFrontend is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GpgFrontend is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GpgFrontend. If not, see <https://www.gnu.org/licenses/>.
 *
 * The initial version of the source code is inherited from
 * the gpg4usb project, which is under GPL-3.0-or-later.
 *
 * All the source code of GpgFrontend was modified and released by
 * Saturneric <eric@bktus.com> starting on May 12, 2021.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <any>
#include <typeindex>
#include <typeinfo>

#include "core/GpgFrontendCoreExport.h"
#include "core/utils/MemoryUtils.h"

namespace GpgFrontend {

class DataObject;
using DataObjectPtr = std::shared_ptr<DataObject>;  ///<

class GPGFRONTEND_CORE_EXPORT DataObject {
 public:
  DataObject();

  DataObject(std::initializer_list<std::any>);

  ~DataObject();

  DataObject(DataObject&&) noexcept;

  auto operator[](size_t index) const -> std::any;

  void AppendObject(std::any);

  [[nodiscard]] auto GetParameter(size_t index) const -> std::any;

  [[nodiscard]] auto GetObjectSize() const -> size_t;

  void Swap(DataObject& other) noexcept;

  void Swap(DataObject&& other) noexcept;

  template <typename... Args>
  auto Check() -> bool {
    if (sizeof...(Args) != GetObjectSize()) return false;

    std::vector<std::type_info const*> type_list = {&typeid(Args)...};
    for (size_t i = 0; i < type_list.size(); ++i) {
      if (std::type_index(*type_list[i]) !=
          std::type_index((*this)[i].type())) {
        SPDLOG_ERROR(
            "value of index {} in data object is type: {}, "
            "not expected type: {}",
            i, ((*this)[i]).type().name(), type_list[i]->name());
        return false;
      }
    }
    return true;
  }

 private:
  class Impl;
  SecureUniquePtr<Impl> p_;
};

template <typename... Args>
auto TransferParams(Args&&... args) -> std::shared_ptr<DataObject> {
  return GpgFrontend::SecureCreateSharedObject<DataObject>(
      DataObject{std::forward<Args>(args)...});
}

template <typename T>
auto ExtractParams(const std::shared_ptr<DataObject>& d_o, int index) -> T {
  if (!d_o) {
    throw std::invalid_argument("nullptr provided for DataObjectPtr");
  }
  return std::any_cast<T>(d_o->GetParameter(index));
}

void swap(DataObject& a, DataObject& b) noexcept;

}  // namespace GpgFrontend