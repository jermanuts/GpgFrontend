/**
 * Copyright (C) 2021 Saturneric
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
 * The source code version of this software was modified and released
 * by Saturneric<eric@bktus.com><eric@bktus.com> starting on May 12, 2021.
 *
 */

#include "CtxCheckThread.h"

#include "gpg/GpgContext.h"
#include "gpg/GpgCoreInit.h"
#include "gpg/function/GpgKeyGetter.h"
#include "ui/UserInterfaceUtils.h"

GpgFrontend::UI::CtxCheckThread::CtxCheckThread() : QThread(nullptr) {
  connect(this, &CtxCheckThread::SignalGnupgNotInstall,
          CommonUtils::GetInstance(), &CommonUtils::SignalGnupgNotInstall);
}

void GpgFrontend::UI::CtxCheckThread::run() {
  // Init GpgFrontend Core
  init_gpgfrontend_core();

  // Create & Check Gnupg Context Status
  if (!GpgContext::GetInstance().good()) {
    emit SignalGnupgNotInstall();
  }
  // Try fetching key
  else
    GpgFrontend::GpgKeyGetter::GetInstance().FetchKey();
}
