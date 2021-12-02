/**
 * This file is part of GpgFrontend.
 *
 * GpgFrontend is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Foobar is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <https://www.gnu.org/licenses/>.
 *
 * The initial version of the source code is inherited from gpg4usb-team.
 * Their source code version also complies with GNU General Public License.
 *
 * The source code version of this software was modified and released
 * by Saturneric<eric@bktus.com> starting on May 12, 2021.
 *
 */

#ifndef GPGFRONTEND_GLOBALSETTINGSTATION_H
#define GPGFRONTEND_GLOBALSETTINGSTATION_H

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <libconfig.h++>

#include "ui/GpgFrontendUI.h"

namespace GpgFrontend::UI {

class GlobalSettingStation : public QObject {
  Q_OBJECT
 public:
  static GlobalSettingStation& GetInstance();

  GlobalSettingStation() noexcept;

  libconfig::Setting& GetUISettings() noexcept { return ui_cfg.getRoot(); }

  [[nodiscard]] boost::filesystem::path GetAppDir() const { return app_path; }

  [[nodiscard]] boost::filesystem::path GetLogDir() const {
    return app_log_path;
  }

  [[nodiscard]] boost::filesystem::path GetLocaleDir() const {
    return app_locale_path;
  }

  void Sync() noexcept;

 private:
  // Program Location
  boost::filesystem::path app_path = qApp->applicationDirPath().toStdString();

  // Program Data Location
  boost::filesystem::path app_data_path =
      QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)
          .toStdString();

  // Program Data Location
  boost::filesystem::path app_log_path = app_data_path / "logs";

  // Program Data Location
  boost::filesystem::path app_locale_path =
      RESOURCE_DIR_BOOST_PATH(app_path) / "locales";

  // Program Configure Location
  boost::filesystem::path app_configure_path =
      QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation)
          .toStdString();

  // Configure File Directory Location
  boost::filesystem::path ui_config_dir_path =
      app_configure_path / "UserInterface";

  // UI Configure File Location
  boost::filesystem::path ui_config_path = ui_config_dir_path / "ui.cfg";

  libconfig::Config ui_cfg;

  static std::unique_ptr<GlobalSettingStation> _instance;
};
}  // namespace GpgFrontend::UI

#endif  // GPGFRONTEND_GLOBALSETTINGSTATION_H
