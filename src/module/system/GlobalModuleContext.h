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

#ifndef GPGFRONTEND_GLOBALMODULECONTEXT_H
#define GPGFRONTEND_GLOBALMODULECONTEXT_H

#include "GpgFrontendModuleSystemExport.h"
#include "core/GpgFrontendCore.h"
#include "core/thread/TaskRunner.h"
#include "module/system/Event.h"

namespace GpgFrontend::Module {

class GlobalModuleContext;

class Module;
class ModuleManager;
using ModuleIdentifier = std::string;
using ModulePtr = std::shared_ptr<Module>;

using GlobalModuleContextPtr = std::shared_ptr<GlobalModuleContext>;

using TaskRunnerPtr = std::shared_ptr<Thread::TaskRunner>;

class GPGFRONTEND_MODULE_SYSTEM_EXPORT GlobalModuleContext : public QObject {
  Q_OBJECT
 public:
  GlobalModuleContext(TaskRunnerPtr);

  ~GlobalModuleContext();

  int GetChannel(ModulePtr);

  int GetDefaultChannel(ModulePtr);

  std::optional<TaskRunnerPtr> GetTaskRunner(ModulePtr);

  std::optional<TaskRunnerPtr> GetTaskRunner(ModuleIdentifier);

  std::optional<TaskRunnerPtr> GetGlobalTaskRunner();

  bool RegisterModule(ModulePtr);

  bool ActiveModule(ModuleIdentifier);

  bool DeactivateModule(ModuleIdentifier);

  bool ListenEvent(ModuleIdentifier, EventIdentifier);

  bool TriggerEvent(EventRefrernce);

 private:
  class Impl;
  std::unique_ptr<Impl> p_;
};

}  // namespace GpgFrontend::Module

#endif  // GPGFRONTEND_GLOBALMODULECONTEXT_H