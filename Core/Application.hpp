/***********************************************************************************
** MIT License                                                                    **
**                                                                                **
** Copyright (c) 2018 Victor DENIS (victordenis01@gmail.com)                      **
**                                                                                **
** Permission is hereby granted, free of charge, to any person obtaining a copy   **
** of this software and associated documentation files (the "Software"), to deal  **
** in the Software without restriction, including without limitation the rights   **
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell      **
** copies of the Software, and to permit persons to whom the Software is          **
** furnished to do so, subject to the following conditions:                       **
**                                                                                **
** The above copyright notice and this permission notice shall be included in all **
** copies or substantial portions of the Software.                                **
**                                                                                **
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     **
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,       **
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE    **
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER         **
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  **
** OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE  **
** SOFTWARE.                                                                      **
***********************************************************************************/

#pragma once
#ifndef CORE_APPLICATION_HPP
#define CORE_APPLICATION_HPP

#include <QApplication>
#include <QList>

#include <QPointer>

#include <QFont>

#include <QWebEngineProfile>

#include "Utils/RestoreManager.hpp"
#include "3rdparty/singleapplication.h"

namespace Sn {
class AutoFill;
class CookieJar;
class PluginProxy;
class HistoryManager;
class BookmarksManager;
class DownloadManager;
class HTML5PermissionsManager;
class NetworkManager;

class BrowserWindow;

//! The This class manage Sielo instance, windows and settings.
/*!
 * This application give access to all settings and informations on Sielo. There is usually only **one** instance of this class for the entire browser
 *
 * This class will also hande OS signals like new tab request, links open request, etc.
 *
 */
class Q_DECL_EXPORT Application : public SingleApplication {
public:
	//! Command line actions
	/*! All command line option that can be requested to Sielo. This can be request from OS. */
	enum CommandLineAction {
		CL_NoAction, /*!< No action requested */
		CL_OpenUrl, /*!< We want to open a URL */
		CL_OpenUrlInCurrentTab, /*!< We want to open the url in the current tab */
		CL_OpenUrlInNewWindow, /*!< We want to open the url in a new window */
		CL_NewTab, /*!< We want to open a new tab */
		CL_NewWindow, /*!< We want to open a new window */
		CL_StartPrivateBrowsing, /*!< We want to open a new private browsing window */
		CL_StartNewInstance, /*!< We want to start a new instance of Sielo */
		CL_ExitAction /*!< We want to close Sielo */
	};

	//! Object name
	/*! Name of object accessible for applications. It can be used when one of this object emit a signal for all applications running. */
	enum ObjectName {
		ON_WebView, /*!< Web view object */
		ON_TabBar, /*!< Tab bar object */
		ON_BrowserWindow /*!< Browser window object */
	};

	//! Window type
	/*! Describe the type of the window. */
	enum WindowType {
		WT_FirstAppWindow, /*!< The window is the first window of the application */
		WT_NewWindow, /*!< The window is a simple new window */
		WT_OtherRestoredWindow /*!< The window is a restored window */
	};

	//! Tabs space type
	/*! Type of the tabs space */
	enum TabsSpaceType {
		TST_Web, /*!< The tabs space run application */
		TST_Application /*!< The tabs space run web pages */
	};

	//! Tab type
	/*! Type of the new tab */
	enum NewTabType {
		NTT_SelectedTab = 1, /*!< The tab will be selected */
		NTT_NotSelectedTab = 2, /*!< The tab will not be selected */
		NTT_CleanTab = 4, /*!< The tab will be clean */
		NTT_TabAtEnd = 8, /*!< The tab will be at the end */
		NTT_NewEmptyTab = 16, /*!< The tab will be blank */
		/* ------------------------- */
				NTT_SelectedNewEmptyTab = NTT_SelectedTab | NTT_TabAtEnd | NTT_NewEmptyTab,
		NTT_SelectedTabAtEnd = NTT_SelectedTab | NTT_TabAtEnd,
		NTT_NotSelectedTabAtEnd = NTT_NotSelectedTab | NTT_TabAtEnd,
		NTT_CleanSelectedTabAtEnd = NTT_SelectedTab | NTT_TabAtEnd | NTT_CleanTab,
		NTT_CleanSelectedTab = NTT_CleanTab | NTT_SelectedTab,
		NTT_CleanNotSelectedTab = NTT_CleanTab | NTT_NotSelectedTab
	};
	Q_DECLARE_FLAGS(NewTabTypeFlags, NewTabType);

	//! Paths
	/*! Path of differents Sielo's needed folders */
	enum Path {
		P_Data = 0, /*!< Path of data */
		P_Plugin = 1, /*!< Path of plugin (will be applications) */
		P_Themes = 2 /*!< Path of themes */
	};

	//! After launch action
	/*! Action that should be exectued after Sielo startup */
	enum AfterLaunch {
		OpenBlankPage = 0, /*!< Open a blank page */
		OpenHomePage = 1, /*!< Open the home page */
		RestoreSession = 2, /*!< Restore the previous session */
		OpenSavedSession = 3 /*!< Restore the saved session */
	};

	/*!
	 * This constructor call functions to load SQL database and load settings.
	 *
	 * It will also determine command line options given and check for updates
	 */
	Application(int& argc, char** argv);
	/*!
	 * The destructor first request to window to save the current session.
	 *
	 * The it remove any window system resources thar were allocated by this application, and free some managers.
	 */
	~Application();

	void loadSettings();
	/*!
	 * Apply a theme to Sielo.
	 * @param name Name of the theme we want to apply.
	 * @param lightness Needed to let the theme know if it should load light or dark icons.
	 */
	void loadTheme(const QString& name, const QString& lightness = "dark");

	bool privateBrowsing() const { return m_privateBrowsing; }

	bool isPortable() const { return m_isPortable; }

	bool isClosing() const { return m_isClosing; }

	int windowCount() const;

	QList<BrowserWindow*> windows() const { return m_windows; }

	BrowserWindow* getWindow() const;
	/*!
	 * Create a new window in this instance.
	 * @param type The type of the new window.
	 * @param startUrl The start url for the new window (if it's empty, it will take the home page url)
	 * @return The new window created
	 */
	BrowserWindow* createWindow(Application::WindowType type, const QUrl& startUrl = QUrl());

	AfterLaunch afterLaunch() const;

	bool restoreSession(BrowserWindow* window, RestoreData restoreData);
	void destroyRestoreManager();

	PluginProxy* plugins() const { return m_plugins; }

	AutoFill* autoFill() const { return m_autoFill; }

	CookieJar* cookieJar();
	HistoryManager* historyManager();
	BookmarksManager* bookmarksManager();
	DownloadManager* downloadManager();
	HTML5PermissionsManager* permissionsManager();

	NetworkManager* networkManager() const { return m_networkManager; }

	RestoreManager* restoreManager() const { return m_restoreManager; }

	QWebEngineProfile* webProfile() const { return m_webProfile; }

	bool fullyLoadThemes() const { return m_fullyLoadThemes; }

	bool useTopToolBar() const { return m_useTopToolBar; }

	bool hideBookmarksHistoryActions() const { return m_hideBookmarksHistoryActions; }

	bool floatingButtonFoloweMouse() const { return m_floatingButtonFoloweMouse; }

	void connectDatabase();

	QString ensureUniqueFilename(const QString& name, const QString& appendFormat = QString("(%1)"));

	QFont morpheusFont() const { return m_morpheusFont; }

	QFont normalFont() const { return m_normalFont; }

	bool copyPath(const QString& fromDir, const QString& toDir, bool coverFileIfExist = true);
	QString readFile(const QString& filename);

	/*!
	 * Process a command in Sielo. There is serval command available for now :
	 *
	 *  - site : open Sielo site.
	 *  - github : open the Sielo repository.
	 *  - witcher [enable/disable] : enable or disable witcher font in the browser.
	 *  - easteregg : open a random crasy site.
	 * @param command The command name
	 * @param args The command arguments
	 */
	void processCommand(const QString& command, const QStringList args);

	static QString currentVersion;
	static QList<QString> paths();
	static Application* instance();
	static QIcon getAppIcon(const QString& name, const QString& defaultDire = "other", const QString& format = ".png");

public slots:
	/*!
	 * Add a new tab to the current tabs space
	 * @param url The url open in the new tab. By default it's the user new tab url.
	 */
	void addNewTab(const QUrl& url = QUrl());
	/*!
	 * Start a new private browsing instance.
	 * @param startUrl Url open in the new private browsing window. By default it's the user home page url.
	 */
	void startPrivateBrowsing(const QUrl& startUrl = QUrl());

	/*!
	 * Save application settings
	 */
	void saveSettings();
	/*!
	 * Save current session.
	 * @param saveForHome If true, the saved session will be the user saved session, else, it will be the restore session.
	 */
	void saveSession(bool saveForHome = false);

	void reloadUserStyleSheet();

	void quitApplication();

private slots:
	void postLaunch();

	void messageReceived(quint32 instanceId, QByteArray messageBytes);
	void windowDestroyed(QObject* window);

	void downloadRequested(QWebEngineDownloadItem* download);
private:
	enum PostLaunchAction {
		OpenNewTab
	};

	void setUserStyleSheet(const QString& filePath);

	void loadThemeFromResources(QString name = "sielo-default", bool loadAtEnd = true);

	bool m_privateBrowsing{false};
	bool m_isPortable{true};
	bool m_isRestoring{false};
	bool m_isClosing{false};
	bool m_fullyLoadThemes{true};
	bool m_useTopToolBar{false};
	bool m_hideBookmarksHistoryActions{false};
	bool m_floatingButtonFoloweMouse{true};
	bool m_databaseConnected{false};

	PluginProxy* m_plugins{nullptr};
	AutoFill* m_autoFill{nullptr};
	CookieJar* m_cookieJar{nullptr};
	HistoryManager* m_historyManager{nullptr};
	BookmarksManager* m_bookmarksManager{nullptr};
	DownloadManager* m_downloadManager{nullptr};
	HTML5PermissionsManager* m_permissionsManager{nullptr};

	NetworkManager* m_networkManager{nullptr};
	QWebEngineProfile* m_webProfile{nullptr};

	RestoreManager* m_restoreManager{nullptr};

	QList<BrowserWindow*> m_windows;
	QPointer<BrowserWindow> m_lastActiveWindow;

	QList<PostLaunchAction> m_postLaunchActions;

	QFont m_morpheusFont{};
	QFont m_normalFont{};
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Application::NewTabTypeFlags);

}
#endif // CORE_APPLICATION_HPP
