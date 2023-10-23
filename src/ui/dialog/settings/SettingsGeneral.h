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

#include "ui/GpgFrontendUI.h"

class Ui_GeneralSettings;

namespace GpgFrontend::UI {
class KeyList;

/**
 * @brief
 *
 */
class GeneralTab : public QWidget {
  Q_OBJECT

 public:
  /**
   * @brief Construct a new General Tab object
   *
   * @param parent
   */
  explicit GeneralTab(QWidget* parent = nullptr);

  /**
   * @brief Set the Settings object
   *
   */
  void SetSettings();

  /**
   * @brief
   *
   */
  void ApplySettings();

 signals:

  /**
   * @brief
   *
   * @param needed
   */
  void SignalRestartNeeded(bool needed);

  /**
   * @brief
   *
   * @param needed
   */
  void SignalDeepRestartNeeded(bool needed);

 private:
  std::shared_ptr<Ui_GeneralSettings> ui_;  ///<

#ifdef SUPPORT_MULTI_LANG
  QHash<QString, QString> lang_;  ///<
#endif

  std::vector<std::string> key_ids_list_;  ///<

  KeyList* m_key_list_{};  ///<

 private slots:

#ifdef SUPPORT_MULTI_LANG
  /**
   * @brief
   *
   */
  void slot_language_changed();
#endif
};
}  // namespace GpgFrontend::UI
