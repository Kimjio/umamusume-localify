#pragma once

#include <windows.h>
#include <cstdint>

#define CEF_CALLBACK __stdcall
#define cef_window_handle_t HWND

// Build with the UTF8 string type as default.
// #define CEF_STRING_TYPE_UTF8 1

// Build with the UTF16 string type as default.
#define CEF_STRING_TYPE_UTF16 1

// Build with the wide string type as default.
// #define CEF_STRING_TYPE_WIDE 1

typedef struct _cef_string_wide_t {
	wchar_t* str;
	size_t length;
	void (*dtor)(wchar_t* str);
} cef_string_wide_t;

typedef struct _cef_string_utf8_t {
	char* str;
	size_t length;
	void (*dtor)(char* str);
} cef_string_utf8_t;

typedef struct _cef_string_utf16_t {
	char16_t* str;
	size_t length;
	void (*dtor)(char16_t* str);
} cef_string_utf16_t;

typedef cef_string_wide_t* cef_string_userfree_wide_t;
typedef cef_string_utf8_t* cef_string_userfree_utf8_t;
typedef cef_string_utf16_t* cef_string_userfree_utf16_t;

#if defined(CEF_STRING_TYPE_UTF8)
typedef char cef_char_t;
typedef cef_string_utf8_t cef_string_t;
typedef cef_string_userfree_utf8_t cef_string_userfree_t;
#elif defined(CEF_STRING_TYPE_UTF16)
typedef char16_t cef_char_t;
typedef cef_string_utf16_t cef_string_t;
typedef cef_string_userfree_utf16_t cef_string_userfree_t;
#elif defined(CEF_STRING_TYPE_WIDE)
typedef wchar_t cef_char_t;
typedef cef_string_wide_t cef_string_t;
typedef cef_string_userfree_wide_t cef_string_userfree_t;
#endif

///
/// These functions set string values. If |copy| is true (1) the value will be
/// copied instead of referenced. It is up to the user to properly manage
/// the lifespan of references.
///

int (*cef_string_wide_set)(const wchar_t* src,
	size_t src_len,
	cef_string_wide_t* output,
	BOOL copy);
int (*cef_string_utf8_set)(const char* src,
	size_t src_len,
	cef_string_utf8_t* output,
	BOOL copy);
int (*cef_string_utf16_set)(const char16_t* src,
	size_t src_len,
	cef_string_utf16_t* output,
	BOOL copy);


#if defined(CEF_STRING_TYPE_UTF8)
#define cef_string_set cef_string_utf8_set;
#elif defined(CEF_STRING_TYPE_UTF16)
#define cef_string_set cef_string_utf16_set;
#elif defined(CEF_STRING_TYPE_WIDE)
#define cef_string_set cef_string_wide_set;
#endif

///
/// CEF string maps are a set of key/value string pairs.
///
typedef struct _cef_string_list_t* cef_string_list_t;

///
/// Allocate a new string map.
///
cef_string_list_t(*cef_string_list_alloc)(void);

///
/// Return the number of elements in the string list.
///
size_t(*cef_string_list_size)(cef_string_list_t list);

///
/// Retrieve the value at the specified zero-based string list index. Returns
/// true (1) if the value was successfully retrieved.
///
int (*cef_string_list_value)(cef_string_list_t list,
	size_t index,
	cef_string_t* value);

///
/// Append a new value at the end of the string list.
///
void (*cef_string_list_append)(cef_string_list_t list,
	const cef_string_t* value);

///
/// Clear the string list.
///
void (*cef_string_list_clear)(cef_string_list_t list);

///
/// Free the string list.
///
void (*cef_string_list_free)(cef_string_list_t list);

///
/// Creates a copy of an existing string list.
///
cef_string_list_t(*cef_string_list_copy)(cef_string_list_t list);

// 32-bit ARGB color value, not premultiplied. The color components are always
// in a known order. Equivalent to the SkColor type.
typedef uint32_t cef_color_t;

// Return the alpha byte from a cef_color_t value.
#define CefColorGetA(color) (((color) >> 24) & 0xFF)
// Return the red byte from a cef_color_t value.
#define CefColorGetR(color) (((color) >> 16) & 0xFF)
// Return the green byte from a cef_color_t value.
#define CefColorGetG(color) (((color) >> 8) & 0xFF)
// Return the blue byte from a cef_color_t value.
#define CefColorGetB(color) (((color) >> 0) & 0xFF)

// Return an cef_color_t value with the specified byte component values.
#define CefColorSetARGB(a, r, g, b)                                         \
  static_cast<cef_color_t>(                                                 \
      (static_cast<unsigned>(a) << 24) | (static_cast<unsigned>(r) << 16) | \
      (static_cast<unsigned>(g) << 8) | (static_cast<unsigned>(b) << 0))

// Return an int64_t value with the specified low and high int32_t component
// values.
#define CefInt64Set(int32_low, int32_high) \
  static_cast<int64_t>(                    \
      (static_cast<uint32_t>(int32_low)) | \
      (static_cast<int64_t>(static_cast<int32_t>(int32_high))) << 32)

// Return the low int32_t value from an int64_t value.
#define CefInt64GetLow(int64_val) static_cast<int32_t>(int64_val)
// Return the high int32_t value from an int64_t value.
#define CefInt64GetHigh(int64_val) \
  static_cast<int32_t>((static_cast<int64_t>(int64_val) >> 32) & 0xFFFFFFFFL)

///
/// Log severity levels.
///
typedef enum {
	///
	/// Default logging (currently INFO logging).
	///
	LOGSEVERITY_DEFAULT,

	///
	/// Verbose logging.
	///
	LOGSEVERITY_VERBOSE,

	///
	/// DEBUG logging.
	///
	LOGSEVERITY_DEBUG = LOGSEVERITY_VERBOSE,

	///
	/// INFO logging.
	///
	LOGSEVERITY_INFO,

	///
	/// WARNING logging.
	///
	LOGSEVERITY_WARNING,

	///
	/// ERROR logging.
	///
	LOGSEVERITY_ERROR,

	///
	/// FATAL logging.
	///
	LOGSEVERITY_FATAL,

	///
	/// Disable logging to file for all messages, and to stderr for messages with
	/// severity less than FATAL.
	///
	LOGSEVERITY_DISABLE = 99
} cef_log_severity_t;

///
/// Log items prepended to each log line.
///
typedef enum {
	///
	/// Prepend the default list of items.
	///
	LOG_ITEMS_DEFAULT = 0,

	///
	/// Prepend no items.
	///
	LOG_ITEMS_NONE = 1,

	///
	/// Prepend the process ID.
	///
	LOG_ITEMS_FLAG_PROCESS_ID = 1 << 1,

	///
	/// Prepend the thread ID.
	///
	LOG_ITEMS_FLAG_THREAD_ID = 1 << 2,

	///
	/// Prepend the timestamp.
	///
	LOG_ITEMS_FLAG_TIME_STAMP = 1 << 3,

	///
	/// Prepend the tickcount.
	///
	LOG_ITEMS_FLAG_TICK_COUNT = 1 << 4,

} cef_log_items_t;

///
/// Represents the state of a setting.
///
typedef enum {
	///
	/// Use the default state for the setting.
	///
	STATE_DEFAULT = 0,

	///
	/// Enable or allow the setting.
	///
	STATE_ENABLED,

	///
	/// Disable or disallow the setting.
	///
	STATE_DISABLED,
} cef_state_t;

///
/// Existing process IDs.
///
typedef enum {
	///
	/// Browser process.
	///
	PID_BROWSER,
	///
	/// Renderer process.
	///
	PID_RENDERER,
} cef_process_id_t;

///
/// Supported value types.
///
typedef enum {
	VTYPE_INVALID = 0,
	VTYPE_NULL,
	VTYPE_BOOL,
	VTYPE_INT,
	VTYPE_DOUBLE,
	VTYPE_STRING,
	VTYPE_BINARY,
	VTYPE_DICTIONARY,
	VTYPE_LIST,
} cef_value_type_t;

///
/// Supported content setting types. Some types are platform-specific or only
/// supported with the Chrome runtime. Should be kept in sync with Chromium's
/// ContentSettingsType type.
///
typedef enum {
	CEF_CONTENT_SETTING_TYPE_COOKIES = 0,
	CEF_CONTENT_SETTING_TYPE_IMAGES,
	CEF_CONTENT_SETTING_TYPE_JAVASCRIPT,

	/// This setting governs both popups and unwanted redirects like tab-unders
	/// and framebusting.
	CEF_CONTENT_SETTING_TYPE_POPUPS,

	CEF_CONTENT_SETTING_TYPE_GEOLOCATION,
	CEF_CONTENT_SETTING_TYPE_NOTIFICATIONS,
	CEF_CONTENT_SETTING_TYPE_AUTO_SELECT_CERTIFICATE,
	CEF_CONTENT_SETTING_TYPE_MIXEDSCRIPT,
	CEF_CONTENT_SETTING_TYPE_MEDIASTREAM_MIC,
	CEF_CONTENT_SETTING_TYPE_MEDIASTREAM_CAMERA,
	CEF_CONTENT_SETTING_TYPE_PROTOCOL_HANDLERS,
	CEF_CONTENT_SETTING_TYPE_DEPRECATED_PPAPI_BROKER,
	CEF_CONTENT_SETTING_TYPE_AUTOMATIC_DOWNLOADS,
	CEF_CONTENT_SETTING_TYPE_MIDI_SYSEX,
	CEF_CONTENT_SETTING_TYPE_SSL_CERT_DECISIONS,
	CEF_CONTENT_SETTING_TYPE_PROTECTED_MEDIA_IDENTIFIER,
	CEF_CONTENT_SETTING_TYPE_APP_BANNER,
	CEF_CONTENT_SETTING_TYPE_SITE_ENGAGEMENT,
	CEF_CONTENT_SETTING_TYPE_DURABLE_STORAGE,
	CEF_CONTENT_SETTING_TYPE_USB_CHOOSER_DATA,
	CEF_CONTENT_SETTING_TYPE_BLUETOOTH_GUARD,
	CEF_CONTENT_SETTING_TYPE_BACKGROUND_SYNC,
	CEF_CONTENT_SETTING_TYPE_AUTOPLAY,
	CEF_CONTENT_SETTING_TYPE_IMPORTANT_SITE_INFO,
	CEF_CONTENT_SETTING_TYPE_PERMISSION_AUTOBLOCKER_DATA,
	CEF_CONTENT_SETTING_TYPE_ADS,

	/// Website setting which stores metadata for the subresource filter to aid in
	/// decisions for whether or not to show the UI.
	CEF_CONTENT_SETTING_TYPE_ADS_DATA,

	/// This is special-cased in the permissions layer to always allow, and as
	/// such doesn't have associated prefs data.
	CEF_CONTENT_SETTING_TYPE_MIDI,

	/// This content setting type is for caching password protection service's
	/// verdicts of each origin.
	CEF_CONTENT_SETTING_TYPE_PASSWORD_PROTECTION,

	/// Website setting which stores engagement data for media related to a
	/// specific origin.
	CEF_CONTENT_SETTING_TYPE_MEDIA_ENGAGEMENT,

	/// Content setting which stores whether or not the site can play audible
	/// sound. This will not block playback but instead the user will not hear it.
	CEF_CONTENT_SETTING_TYPE_SOUND,

	/// Website setting which stores the list of client hints that the origin
	/// requested the browser to remember. The browser is expected to send all
	/// client hints in the HTTP request headers for every resource requested
	/// from that origin.
	CEF_CONTENT_SETTING_TYPE_CLIENT_HINTS,

	/// Generic Sensor API covering ambient-light-sensor, accelerometer, gyroscope
	/// and magnetometer are all mapped to a single content_settings_type.
	/// Setting for the Generic Sensor API covering ambient-light-sensor,
	/// accelerometer, gyroscope and magnetometer. These are all mapped to a
	/// single ContentSettingsType.
	CEF_CONTENT_SETTING_TYPE_SENSORS,

	/// Content setting which stores whether or not the user has granted the site
	/// permission to respond to accessibility events, which can be used to
	/// provide a custom accessibility experience. Requires explicit user consent
	/// because some users may not want sites to know they're using assistive
	/// technology.
	CEF_CONTENT_SETTING_TYPE_ACCESSIBILITY_EVENTS,

	/// Used to store whether to allow a website to install a payment handler.
	CEF_CONTENT_SETTING_TYPE_PAYMENT_HANDLER,

	/// Content setting which stores whether to allow sites to ask for permission
	/// to access USB devices. If this is allowed specific device permissions are
	/// stored under USB_CHOOSER_DATA.
	CEF_CONTENT_SETTING_TYPE_USB_GUARD,

	/// Nothing is stored in this setting at present. Please refer to
	/// BackgroundFetchPermissionContext for details on how this permission
	/// is ascertained.
	CEF_CONTENT_SETTING_TYPE_BACKGROUND_FETCH,

	/// Website setting which stores the amount of times the user has dismissed
	/// intent picker UI without explicitly choosing an option.
	CEF_CONTENT_SETTING_TYPE_INTENT_PICKER_DISPLAY,

	/// Used to store whether to allow a website to detect user active/idle state.
	CEF_CONTENT_SETTING_TYPE_IDLE_DETECTION,

	/// Setting for enabling auto-select of all screens for getDisplayMediaSet.
	CEF_CONTENT_SETTING_TYPE_GET_DISPLAY_MEDIA_SET_SELECT_ALL_SCREENS,

	/// Content settings for access to serial ports. The "guard" content setting
	/// stores whether to allow sites to ask for permission to access a port. The
	/// permissions granted to access particular ports are stored in the "chooser
	/// data" website setting.
	CEF_CONTENT_SETTING_TYPE_SERIAL_GUARD,
	CEF_CONTENT_SETTING_TYPE_SERIAL_CHOOSER_DATA,

	/// Nothing is stored in this setting at present. Please refer to
	/// PeriodicBackgroundSyncPermissionContext for details on how this permission
	/// is ascertained.
	/// This content setting is not registered because it does not require access
	/// to any existing providers.
	CEF_CONTENT_SETTING_TYPE_PERIODIC_BACKGROUND_SYNC,

	/// Content setting which stores whether to allow sites to ask for permission
	/// to do Bluetooth scanning.
	CEF_CONTENT_SETTING_TYPE_BLUETOOTH_SCANNING,

	/// Content settings for access to HID devices. The "guard" content setting
	/// stores whether to allow sites to ask for permission to access a device.
	/// The permissions granted to access particular devices are stored in the
	/// "chooser data" website setting.
	CEF_CONTENT_SETTING_TYPE_HID_GUARD,
	CEF_CONTENT_SETTING_TYPE_HID_CHOOSER_DATA,

	/// Wake Lock API, which has two lock types: screen and system locks.
	/// Currently, screen locks do not need any additional permission, and system
	/// locks are always denied while the right UI is worked out.
	CEF_CONTENT_SETTING_TYPE_WAKE_LOCK_SCREEN,
	CEF_CONTENT_SETTING_TYPE_WAKE_LOCK_SYSTEM,

	/// Legacy SameSite cookie behavior. This disables SameSite=Lax-by-default,
	/// SameSite=None requires Secure, and Schemeful Same-Site, forcing the
	/// legacy behavior wherein 1) cookies that don't specify SameSite are treated
	/// as SameSite=None, 2) SameSite=None cookies are not required to be Secure,
	/// and 3) schemeful same-site is not active.
	///
	/// This will also be used to revert to legacy behavior when future changes
	/// in cookie handling are introduced.
	CEF_CONTENT_SETTING_TYPE_LEGACY_COOKIE_ACCESS,

	/// Content settings which stores whether to allow sites to ask for permission
	/// to save changes to an original file selected by the user through the
	/// File System Access API.
	CEF_CONTENT_SETTING_TYPE_FILE_SYSTEM_WRITE_GUARD,

	/// Used to store whether to allow a website to exchange data with NFC
	/// devices.
	CEF_CONTENT_SETTING_TYPE_NFC,

	/// Website setting to store permissions granted to access particular
	/// Bluetooth devices.
	CEF_CONTENT_SETTING_TYPE_BLUETOOTH_CHOOSER_DATA,

	/// Full access to the system clipboard (sanitized read without user gesture,
	/// and unsanitized read and write with user gesture).
	CEF_CONTENT_SETTING_TYPE_CLIPBOARD_READ_WRITE,

	/// This is special-cased in the permissions layer to always allow, and as
	/// such doesn't have associated prefs data.
	CEF_CONTENT_SETTING_TYPE_CLIPBOARD_SANITIZED_WRITE,

	/// This content setting type is for caching safe browsing real time url
	/// check's verdicts of each origin.
	CEF_CONTENT_SETTING_TYPE_SAFE_BROWSING_URL_CHECK_DATA,

	/// Used to store whether a site is allowed to request AR or VR sessions with
	/// the WebXr Device API.
	CEF_CONTENT_SETTING_TYPE_VR,
	CEF_CONTENT_SETTING_TYPE_AR,

	/// Content setting which stores whether to allow site to open and read files
	/// and directories selected through the File System Access API.
	CEF_CONTENT_SETTING_TYPE_FILE_SYSTEM_READ_GUARD,

	/// Access to first party storage in a third-party context. Exceptions are
	/// scoped to the combination of requesting/top-level origin, and are managed
	/// through the Storage Access API. For the time being, this content setting
	/// exists in parallel to third-party cookie rules stored in COOKIES.
	CEF_CONTENT_SETTING_TYPE_STORAGE_ACCESS,

	/// Content setting which stores whether to allow a site to control camera
	/// movements. It does not give access to camera.
	CEF_CONTENT_SETTING_TYPE_CAMERA_PAN_TILT_ZOOM,

	/// Content setting for Screen Enumeration and Screen Detail functionality.
	/// Permits access to detailed multi-screen information, like size and
	/// position. Permits placing fullscreen and windowed content on specific
	/// screens. See also: https://w3c.github.io/window-placement
	CEF_CONTENT_SETTING_TYPE_WINDOW_MANAGEMENT,

	/// Stores whether to allow insecure websites to make local network requests.
	/// See also: https://wicg.github.io/local-network-access
	/// Set through enterprise policies only.
	CEF_CONTENT_SETTING_TYPE_INSECURE_LOCAL_NETWORK,

	/// Content setting which stores whether or not a site can access low-level
	/// locally installed font data using the Local Fonts Access API.
	CEF_CONTENT_SETTING_TYPE_LOCAL_FONTS,

	/// Stores per-origin state for permission auto-revocation (for all permission
	/// types).
	CEF_CONTENT_SETTING_TYPE_PERMISSION_AUTOREVOCATION_DATA,

	/// Stores per-origin state of the most recently selected directory for the
	/// use by the File System Access API.
	CEF_CONTENT_SETTING_TYPE_FILE_SYSTEM_LAST_PICKED_DIRECTORY,

	/// Controls access to the getDisplayMedia API when {preferCurrentTab: true}
	/// is specified.
	CEF_CONTENT_SETTING_TYPE_DISPLAY_CAPTURE,

	/// Website setting to store permissions metadata granted to paths on the
	/// local file system via the File System Access API.
	/// |FILE_SYSTEM_WRITE_GUARD| is the corresponding "guard" setting.
	CEF_CONTENT_SETTING_TYPE_FILE_SYSTEM_ACCESS_CHOOSER_DATA,

	/// Stores a grant that allows a relying party to send a request for identity
	/// information to specified identity providers, potentially through any
	/// anti-tracking measures that would otherwise prevent it. This setting is
	/// associated with the relying party's origin.
	CEF_CONTENT_SETTING_TYPE_FEDERATED_IDENTITY_SHARING,

	/// Whether to use the v8 optimized JIT for running JavaScript on the page.
	CEF_CONTENT_SETTING_TYPE_JAVASCRIPT_JIT,

	/// Content setting which stores user decisions to allow loading a site over
	/// HTTP. Entries are added by hostname when a user bypasses the HTTPS-First
	/// Mode interstitial warning when a site does not support HTTPS. Allowed
	/// hosts are exact hostname matches -- subdomains of a host on the allowlist
	/// must be separately allowlisted.
	CEF_CONTENT_SETTING_TYPE_HTTP_ALLOWED,

	/// Stores metadata related to form fill, such as e.g. whether user data was
	/// autofilled on a specific website.
	CEF_CONTENT_SETTING_TYPE_FORMFILL_METADATA,

	/// Setting to indicate that there is an active federated sign-in session
	/// between a specified relying party and a specified identity provider for
	/// a specified account. When this is present it allows access to session
	/// management capabilities between the sites. This setting is associated
	/// with the relying party's origin.
	CEF_CONTENT_SETTING_TYPE_FEDERATED_IDENTITY_ACTIVE_SESSION,

	/// Setting to indicate whether Chrome should automatically apply darkening to
	/// web content.
	CEF_CONTENT_SETTING_TYPE_AUTO_DARK_WEB_CONTENT,

	/// Setting to indicate whether Chrome should request the desktop view of a
	/// site instead of the mobile one.
	CEF_CONTENT_SETTING_TYPE_REQUEST_DESKTOP_SITE,

	/// Setting to indicate whether browser should allow signing into a website
	/// via the browser FedCM API.
	CEF_CONTENT_SETTING_TYPE_FEDERATED_IDENTITY_API,

	/// Stores notification interactions per origin for the past 90 days.
	/// Interactions per origin are pre-aggregated over seven-day windows: A
	/// notification interaction or display is assigned to the last Monday
	/// midnight in local time.
	CEF_CONTENT_SETTING_TYPE_NOTIFICATION_INTERACTIONS,

	/// Website setting which stores the last reduced accept language negotiated
	/// for a given origin, to be used on future visits to the origin.
	CEF_CONTENT_SETTING_TYPE_REDUCED_ACCEPT_LANGUAGE,

	/// Website setting which is used for NotificationPermissionReviewService to
	/// store origin blocklist from review notification permissions feature.
	CEF_CONTENT_SETTING_TYPE_NOTIFICATION_PERMISSION_REVIEW,

	/// Website setting to store permissions granted to access particular devices
	/// in private network.
	CEF_CONTENT_SETTING_TYPE_PRIVATE_NETWORK_GUARD,
	CEF_CONTENT_SETTING_TYPE_PRIVATE_NETWORK_CHOOSER_DATA,

	/// Website setting which stores whether the browser has observed the user
	/// signing into an identity-provider based on observing the IdP-SignIn-Status
	/// HTTP header.
	CEF_CONTENT_SETTING_TYPE_FEDERATED_IDENTITY_IDENTITY_PROVIDER_SIGNIN_STATUS,

	/// Website setting which is used for UnusedSitePermissionsService to
	/// store revoked permissions of unused sites from unused site permissions
	/// feature.
	CEF_CONTENT_SETTING_TYPE_REVOKED_UNUSED_SITE_PERMISSIONS,

	/// Similar to STORAGE_ACCESS, but applicable at the page-level rather than
	/// being specific to a frame.
	CEF_CONTENT_SETTING_TYPE_TOP_LEVEL_STORAGE_ACCESS,

	/// Setting to indicate whether user has opted in to allowing auto re-authn
	/// via the FedCM API.
	CEF_CONTENT_SETTING_TYPE_FEDERATED_IDENTITY_AUTO_REAUTHN_PERMISSION,

	/// Website setting which stores whether the user has explicitly registered
	/// a website as an identity-provider.
	CEF_CONTENT_SETTING_TYPE_FEDERATED_IDENTITY_IDENTITY_PROVIDER_REGISTRATION,

	/// Content setting which is used to indicate whether anti-abuse functionality
	/// should be enabled.
	CEF_CONTENT_SETTING_TYPE_ANTI_ABUSE,

	/// Content setting used to indicate whether third-party storage partitioning
	/// should be enabled.
	CEF_CONTENT_SETTING_TYPE_THIRD_PARTY_STORAGE_PARTITIONING,

	/// Used to indicate whether HTTPS-First Mode is enabled on the hostname.
	CEF_CONTENT_SETTING_TYPE_HTTPS_ENFORCED,

	CEF_CONTENT_SETTING_TYPE_NUM_TYPES,
} cef_content_setting_types_t;

///
/// Supported content setting values. Should be kept in sync with Chromium's
/// ContentSetting type.
///
typedef enum {
	CEF_CONTENT_SETTING_VALUE_DEFAULT = 0,
	CEF_CONTENT_SETTING_VALUE_ALLOW,
	CEF_CONTENT_SETTING_VALUE_BLOCK,
	CEF_CONTENT_SETTING_VALUE_ASK,
	CEF_CONTENT_SETTING_VALUE_SESSION_ONLY,
	CEF_CONTENT_SETTING_VALUE_DETECT_IMPORTANT_CONTENT,

	CEF_CONTENT_SETTING_VALUE_NUM_VALUES
} cef_content_setting_values_t;

///
/// Chrome toolbar types.
///
typedef enum {
	CEF_CTT_NONE = 1,
	CEF_CTT_NORMAL,
	CEF_CTT_LOCATION,
} cef_chrome_toolbar_type_t;

///
/// Specifies the gesture commands.
///
typedef enum {
	CEF_GESTURE_COMMAND_BACK,
	CEF_GESTURE_COMMAND_FORWARD,
} cef_gesture_command_t;

///
/// Structure representing a point.
///
typedef struct _cef_point_t {
	int x;
	int y;
} cef_point_t;

///
/// Structure representing a rectangle.
///
typedef struct _cef_rect_t {
	int x;
	int y;
	int width;
	int height;
} cef_rect_t;

///
/// Structure representing a size.
///
typedef struct _cef_size_t {
	int width;
	int height;
} cef_size_t;

///
/// Structure representing insets.
///
typedef struct _cef_insets_t {
	int top;
	int left;
	int bottom;
	int right;
} cef_insets_t;

///
/// Initialization settings. Specify NULL or 0 to get the recommended default
/// values. Many of these and other settings can also configured using command-
/// line switches.
///
typedef struct _cef_settings_t {
	///
	/// Size of this structure.
	///
	size_t size;

	///
	/// Set to true (1) to disable the sandbox for sub-processes. See
	/// cef_sandbox_win.h for requirements to enable the sandbox on Windows. Also
	/// configurable using the "no-sandbox" command-line switch.
	///
	int no_sandbox;

	///
	/// The path to a separate executable that will be launched for sub-processes.
	/// If this value is empty on Windows or Linux then the main process
	/// executable will be used. If this value is empty on macOS then a helper
	/// executable must exist at "Contents/Frameworks/<app>
	/// Helper.app/Contents/MacOS/<app> Helper" in the top-level app bundle. See
	/// the comments on CefExecuteProcess() for details. If this value is
	/// non-empty then it must be an absolute path. Also configurable using the
	/// "browser-subprocess-path" command-line switch.
	///
	cef_string_t browser_subprocess_path;

	///
	/// The path to the CEF framework directory on macOS. If this value is empty
	/// then the framework must exist at "Contents/Frameworks/Chromium Embedded
	/// Framework.framework" in the top-level app bundle. If this value is
	/// non-empty then it must be an absolute path. Also configurable using the
	/// "framework-dir-path" command-line switch.
	///
	cef_string_t framework_dir_path;

	///
	/// The path to the main bundle on macOS. If this value is empty then it
	/// defaults to the top-level app bundle. If this value is non-empty then it
	/// must be an absolute path. Also configurable using the "main-bundle-path"
	/// command-line switch.
	///
	cef_string_t main_bundle_path;

	///
	/// Set to true (1) to enable use of the Chrome runtime in CEF. This feature
	/// is considered experimental and is not recommended for most users at this
	/// time. See issue #2969 for details.
	///
	int chrome_runtime;

	///
	/// Set to true (1) to have the browser process message loop run in a separate
	/// thread. If false (0) then the CefDoMessageLoopWork() function must be
	/// called from your application message loop. This option is only supported
	/// on Windows and Linux.
	///
	int multi_threaded_message_loop;

	///
	/// Set to true (1) to control browser process main (UI) thread message pump
	/// scheduling via the CefBrowserProcessHandler::OnScheduleMessagePumpWork()
	/// callback. This option is recommended for use in combination with the
	/// CefDoMessageLoopWork() function in cases where the CEF message loop must
	/// be integrated into an existing application message loop (see additional
	/// comments and warnings on CefDoMessageLoopWork). Enabling this option is
	/// not recommended for most users; leave this option disabled and use either
	/// the CefRunMessageLoop() function or multi_threaded_message_loop if
	/// possible.
	///
	int external_message_pump;

	///
	/// Set to true (1) to enable windowless (off-screen) rendering support. Do
	/// not enable this value if the application does not use windowless rendering
	/// as it may reduce rendering performance on some systems.
	///
	int windowless_rendering_enabled;

	///
	/// Set to true (1) to disable configuration of browser process features using
	/// standard CEF and Chromium command-line arguments. Configuration can still
	/// be specified using CEF data structures or via the
	/// CefApp::OnBeforeCommandLineProcessing() method.
	///
	int command_line_args_disabled;

	///
	/// The location where data for the global browser cache will be stored on
	/// disk. If this value is non-empty then it must be an absolute path that is
	/// either equal to or a child directory of CefSettings.root_cache_path. If
	/// this value is empty then browsers will be created in "incognito mode"
	/// where in-memory caches are used for storage and no data is persisted to
	/// disk. HTML5 databases such as localStorage will only persist across
	/// sessions if a cache path is specified. Can be overridden for individual
	/// CefRequestContext instances via the CefRequestContextSettings.cache_path
	/// value. When using the Chrome runtime the "default" profile will be used if
	/// |cache_path| and |root_cache_path| have the same value.
	///
	cef_string_t cache_path;

	///
	/// The root directory that all CefSettings.cache_path and
	/// CefRequestContextSettings.cache_path values must have in common. If this
	/// value is empty and CefSettings.cache_path is non-empty then it will
	/// default to the CefSettings.cache_path value. If both values are empty
	/// then the default platform-specific directory will be used
	/// ("~/.config/cef_user_data" directory on Linux, "~/Library/Application
	/// Support/CEF/User Data" directory on MacOS, "AppData\Local\CEF\User Data"
	/// directory under the user profile directory on Windows). If this value is
	/// non-empty then it must be an absolute path. Failure to set this value
	/// correctly may result in the sandbox blocking read/write access to certain
	/// files.
	///
	cef_string_t root_cache_path;

	///
	/// To persist session cookies (cookies without an expiry date or validity
	/// interval) by default when using the global cookie manager set this value
	/// to true (1). Session cookies are generally intended to be transient and
	/// most Web browsers do not persist them. A |cache_path| value must also be
	/// specified to enable this feature. Also configurable using the
	/// "persist-session-cookies" command-line switch. Can be overridden for
	/// individual CefRequestContext instances via the
	/// CefRequestContextSettings.persist_session_cookies value.
	///
	int persist_session_cookies;

	///
	/// To persist user preferences as a JSON file in the cache path directory set
	/// this value to true (1). A |cache_path| value must also be specified
	/// to enable this feature. Also configurable using the
	/// "persist-user-preferences" command-line switch. Can be overridden for
	/// individual CefRequestContext instances via the
	/// CefRequestContextSettings.persist_user_preferences value.
	///
	int persist_user_preferences;

	///
	/// Value that will be returned as the User-Agent HTTP header. If empty the
	/// default User-Agent string will be used. Also configurable using the
	/// "user-agent" command-line switch.
	///
	cef_string_t user_agent;

	///
	/// Value that will be inserted as the product portion of the default
	/// User-Agent string. If empty the Chromium product version will be used. If
	/// |userAgent| is specified this value will be ignored. Also configurable
	/// using the "user-agent-product" command-line switch.
	///
	cef_string_t user_agent_product;

	///
	/// The locale string that will be passed to WebKit. If empty the default
	/// locale of "en-US" will be used. This value is ignored on Linux where
	/// locale is determined using environment variable parsing with the
	/// precedence order: LANGUAGE, LC_ALL, LC_MESSAGES and LANG. Also
	/// configurable using the "lang" command-line switch.
	///
	cef_string_t locale;

	///
	/// The directory and file name to use for the debug log. If empty a default
	/// log file name and location will be used. On Windows and Linux a
	/// "debug.log" file will be written in the main executable directory. On
	/// MacOS a "~/Library/Logs/[app name]_debug.log" file will be written where
	/// [app name] is the name of the main app executable. Also configurable using
	/// the "log-file" command-line switch.
	///
	cef_string_t log_file;

	///
	/// The log severity. Only messages of this severity level or higher will be
	/// logged. When set to DISABLE no messages will be written to the log file,
	/// but FATAL messages will still be output to stderr. Also configurable using
	/// the "log-severity" command-line switch with a value of "verbose", "info",
	/// "warning", "error", "fatal" or "disable".
	///
	cef_log_severity_t log_severity;

	///
	/// The log items prepended to each log line. If not set the default log items
	/// will be used. Also configurable using the "log-items" command-line switch
	/// with a value of "none" for no log items, or a comma-delimited list of
	/// values "pid", "tid", "timestamp" or "tickcount" for custom log items.
	///
	cef_log_items_t log_items;

	///
	/// Custom flags that will be used when initializing the V8 JavaScript engine.
	/// The consequences of using custom flags may not be well tested. Also
	/// configurable using the "js-flags" command-line switch.
	///
	cef_string_t javascript_flags;

	///
	/// The fully qualified path for the resources directory. If this value is
	/// empty the *.pak files must be located in the module directory on
	/// Windows/Linux or the app bundle Resources directory on MacOS. If this
	/// value is non-empty then it must be an absolute path. Also configurable
	/// using the "resources-dir-path" command-line switch.
	///
	cef_string_t resources_dir_path;

	///
	/// The fully qualified path for the locales directory. If this value is empty
	/// the locales directory must be located in the module directory. If this
	/// value is non-empty then it must be an absolute path. This value is ignored
	/// on MacOS where pack files are always loaded from the app bundle Resources
	/// directory. Also configurable using the "locales-dir-path" command-line
	/// switch.
	///
	cef_string_t locales_dir_path;

	///
	/// Set to true (1) to disable loading of pack files for resources and
	/// locales. A resource bundle handler must be provided for the browser and
	/// render processes via CefApp::GetResourceBundleHandler() if loading of pack
	/// files is disabled. Also configurable using the "disable-pack-loading"
	/// command- line switch.
	///
	int pack_loading_disabled;

	///
	/// Set to a value between 1024 and 65535 to enable remote debugging on the
	/// specified port. Also configurable using the "remote-debugging-port"
	/// command-line switch. Remote debugging can be accessed by loading the
	/// chrome://inspect page in Google Chrome. Port numbers 9222 and 9229 are
	/// discoverable by default. Other port numbers may need to be configured via
	/// "Discover network targets" on the Devices tab.
	///
	int remote_debugging_port;

	///
	/// The number of stack trace frames to capture for uncaught exceptions.
	/// Specify a positive value to enable the
	/// CefRenderProcessHandler::OnUncaughtException() callback. Specify 0
	/// (default value) and OnUncaughtException() will not be called. Also
	/// configurable using the "uncaught-exception-stack-size" command-line
	/// switch.
	///
	int uncaught_exception_stack_size;

	///
	/// Background color used for the browser before a document is loaded and when
	/// no document color is specified. The alpha component must be either fully
	/// opaque (0xFF) or fully transparent (0x00). If the alpha component is fully
	/// opaque then the RGB components will be used as the background color. If
	/// the alpha component is fully transparent for a windowed browser then the
	/// default value of opaque white be used. If the alpha component is fully
	/// transparent for a windowless (off-screen) browser then transparent
	/// painting will be enabled.
	///
	cef_color_t background_color;

	///
	/// Comma delimited ordered list of language codes without any whitespace that
	/// will be used in the "Accept-Language" HTTP header. May be overridden on a
	/// per-browser basis using the CefBrowserSettings.accept_language_list value.
	/// If both values are empty then "en-US,en" will be used. Can be overridden
	/// for individual CefRequestContext instances via the
	/// CefRequestContextSettings.accept_language_list value.
	///
	cef_string_t accept_language_list;

	///
	/// Comma delimited list of schemes supported by the associated
	/// CefCookieManager. If |cookieable_schemes_exclude_defaults| is false (0)
	/// the default schemes ("http", "https", "ws" and "wss") will also be
	/// supported. Not specifying a |cookieable_schemes_list| value and setting
	/// |cookieable_schemes_exclude_defaults| to true (1) will disable all loading
	/// and saving of cookies. These settings will only impact the global
	/// CefRequestContext. Individual CefRequestContext instances can be
	/// configured via the CefRequestContextSettings.cookieable_schemes_list and
	/// CefRequestContextSettings.cookieable_schemes_exclude_defaults values.
	///
	cef_string_t cookieable_schemes_list;
	int cookieable_schemes_exclude_defaults;
} cef_settings_t;

///
/// Browser initialization settings. Specify NULL or 0 to get the recommended
/// default values. The consequences of using custom values may not be well
/// tested. Many of these and other settings can also configured using command-
/// line switches.
///
typedef struct _cef_browser_settings_t {
	///
	/// Size of this structure.
	///
	size_t size;

	///
	/// The maximum rate in frames per second (fps) that CefRenderHandler::OnPaint
	/// will be called for a windowless browser. The actual fps may be lower if
	/// the browser cannot generate frames at the requested rate. The minimum
	/// value is 1 and the maximum value is 60 (default 30). This value can also
	/// be changed dynamically via CefBrowserHost::SetWindowlessFrameRate.
	///
	int windowless_frame_rate;

	/// BEGIN values that map to WebPreferences settings.

	///
	/// Font settings.
	///
	cef_string_t standard_font_family;
	cef_string_t fixed_font_family;
	cef_string_t serif_font_family;
	cef_string_t sans_serif_font_family;
	cef_string_t cursive_font_family;
	cef_string_t fantasy_font_family;
	int default_font_size;
	int default_fixed_font_size;
	int minimum_font_size;
	int minimum_logical_font_size;

	///
	/// Default encoding for Web content. If empty "ISO-8859-1" will be used. Also
	/// configurable using the "default-encoding" command-line switch.
	///
	cef_string_t default_encoding;

	///
	/// Controls the loading of fonts from remote sources. Also configurable using
	/// the "disable-remote-fonts" command-line switch.
	///
	cef_state_t remote_fonts;

	///
	/// Controls whether JavaScript can be executed. Also configurable using the
	/// "disable-javascript" command-line switch.
	///
	cef_state_t javascript;

	///
	/// Controls whether JavaScript can be used to close windows that were not
	/// opened via JavaScript. JavaScript can still be used to close windows that
	/// were opened via JavaScript or that have no back/forward history. Also
	/// configurable using the "disable-javascript-close-windows" command-line
	/// switch.
	///
	cef_state_t javascript_close_windows;

	///
	/// Controls whether JavaScript can access the clipboard. Also configurable
	/// using the "disable-javascript-access-clipboard" command-line switch.
	///
	cef_state_t javascript_access_clipboard;

	///
	/// Controls whether DOM pasting is supported in the editor via
	/// execCommand("paste"). The |javascript_access_clipboard| setting must also
	/// be enabled. Also configurable using the "disable-javascript-dom-paste"
	/// command-line switch.
	///
	cef_state_t javascript_dom_paste;

	///
	/// Controls whether image URLs will be loaded from the network. A cached
	/// image will still be rendered if requested. Also configurable using the
	/// "disable-image-loading" command-line switch.
	///
	cef_state_t image_loading;

	///
	/// Controls whether standalone images will be shrunk to fit the page. Also
	/// configurable using the "image-shrink-standalone-to-fit" command-line
	/// switch.
	///
	cef_state_t image_shrink_standalone_to_fit;

	///
	/// Controls whether text areas can be resized. Also configurable using the
	/// "disable-text-area-resize" command-line switch.
	///
	cef_state_t text_area_resize;

	///
	/// Controls whether the tab key can advance focus to links. Also configurable
	/// using the "disable-tab-to-links" command-line switch.
	///
	cef_state_t tab_to_links;

	///
	/// Controls whether local storage can be used. Also configurable using the
	/// "disable-local-storage" command-line switch.
	///
	cef_state_t local_storage;

	///
	/// Controls whether databases can be used. Also configurable using the
	/// "disable-databases" command-line switch.
	///
	cef_state_t databases;

	///
	/// Controls whether WebGL can be used. Note that WebGL requires hardware
	/// support and may not work on all systems even when enabled. Also
	/// configurable using the "disable-webgl" command-line switch.
	///
	cef_state_t webgl;

	/// END values that map to WebPreferences settings.

	///
	/// Background color used for the browser before a document is loaded and when
	/// no document color is specified. The alpha component must be either fully
	/// opaque (0xFF) or fully transparent (0x00). If the alpha component is fully
	/// opaque then the RGB components will be used as the background color. If
	/// the alpha component is fully transparent for a windowed browser then the
	/// CefSettings.background_color value will be used. If the alpha component is
	/// fully transparent for a windowless (off-screen) browser then transparent
	/// painting will be enabled.
	///
	cef_color_t background_color;

	///
	/// Comma delimited ordered list of language codes without any whitespace that
	/// will be used in the "Accept-Language" HTTP header. May be set globally
	/// using the CefSettings.accept_language_list value. If both values are
	/// empty then "en-US,en" will be used.
	///
	cef_string_t accept_language_list;

	///
	/// Controls whether the Chrome status bubble will be used. Only supported
	/// with the Chrome runtime. For details about the status bubble see
	/// https://www.chromium.org/user-experience/status-bubble/
	///
	cef_state_t chrome_status_bubble;
} cef_browser_settings_t;

///
// All ref-counted framework structures must include this structure first.
///
typedef struct _cef_base_ref_counted_t {
	///
	// Size of the data structure.
	///
	size_t size;

	///
	// Called to increment the reference count for the object. Should be called
	// for every new copy of a pointer to a given object.
	///
	void(CEF_CALLBACK* add_ref)(struct _cef_base_ref_counted_t* self);

	///
	// Called to decrement the reference count for the object. If the reference
	// count falls to 0 the object should self-delete. Returns true (1) if the
	// resulting reference count is 0.
	///
	int(CEF_CALLBACK* release)(struct _cef_base_ref_counted_t* self);

	///
	// Returns true (1) if the current reference count is 1.
	///
	int(CEF_CALLBACK* has_one_ref)(struct _cef_base_ref_counted_t* self);

	///
	// Returns true (1) if the current reference count is at least 1.
	///
	int(CEF_CALLBACK* has_at_least_one_ref)(struct _cef_base_ref_counted_t* self);
} cef_base_ref_counted_t;

///
/// Implement this structure to provide handler implementations. Methods will be
/// called by the process and/or thread indicated.
///
typedef struct _cef_app_t {
	///
	/// Base structure.
	///
	cef_base_ref_counted_t base;

	///
	/// Provides an opportunity to view and/or modify command-line arguments
	/// before processing by CEF and Chromium. The |process_type| value will be
	/// NULL for the browser process. Do not keep a reference to the
	/// cef_command_line_t object passed to this function. The
	/// cef_settings_t.command_line_args_disabled value can be used to start with
	/// an NULL command-line object. Any values specified in CefSettings that
	/// equate to command-line arguments will be set before this function is
	/// called. Be cautious when using this function to modify command-line
	/// arguments for non-browser processes as this may result in undefined
	/// behavior including crashes.
	///
	void(CEF_CALLBACK* on_before_command_line_processing)(
		struct _cef_app_t* self,
		const cef_string_t* process_type,
		struct _cef_command_line_t* command_line);

	///
	/// Provides an opportunity to register custom schemes. Do not keep a
	/// reference to the |registrar| object. This function is called on the main
	/// thread for each process and the registered schemes should be the same
	/// across all processes.
	///
	void(CEF_CALLBACK* on_register_custom_schemes)(
		struct _cef_app_t* self,
		struct _cef_scheme_registrar_t* registrar);

	///
	/// Return the handler for resource bundle events. If
	/// cef_settings_t.pack_loading_disabled is true (1) a handler must be
	/// returned. If no handler is returned resources will be loaded from pack
	/// files. This function is called by the browser and render processes on
	/// multiple threads.
	///
	struct _cef_resource_bundle_handler_t* (
		CEF_CALLBACK* get_resource_bundle_handler)(struct _cef_app_t* self);

	///
	/// Return the handler for functionality specific to the browser process. This
	/// function is called on multiple threads in the browser process.
	///
	struct _cef_browser_process_handler_t* (
		CEF_CALLBACK* get_browser_process_handler)(struct _cef_app_t* self);

	///
	/// Return the handler for functionality specific to the render process. This
	/// function is called on the render process main thread.
	///
	struct _cef_render_process_handler_t* (
		CEF_CALLBACK* get_render_process_handler)(struct _cef_app_t* self);
} cef_app_t;

typedef struct _cef_main_args_t {
	HINSTANCE instance;
} cef_main_args_t;

///
/// Structure representing window information.
///
typedef struct _cef_window_info_t {
	// Standard parameters required by CreateWindowEx()
	DWORD ex_style;
	cef_string_t window_name;
	DWORD style;
	cef_rect_t bounds;
	cef_window_handle_t parent_window;
	HMENU menu;

	///
	/// Set to true (1) to create the browser using windowless (off-screen)
	/// rendering. No window will be created for the browser and all rendering
	/// will occur via the CefRenderHandler interface. The |parent_window| value
	/// will be used to identify monitor info and to act as the parent window for
	/// dialogs, context menus, etc. If |parent_window| is not provided then the
	/// main screen monitor will be used and some functionality that requires a
	/// parent window may not function correctly. In order to create windowless
	/// browsers the CefSettings.windowless_rendering_enabled value must be set to
	/// true. Transparent painting is enabled by default but can be disabled by
	/// setting CefBrowserSettings.background_color to an opaque value.
	///
	int windowless_rendering_enabled;

	///
	/// Set to true (1) to enable shared textures for windowless rendering. Only
	/// valid if windowless_rendering_enabled above is also set to true. Currently
	/// only supported on Windows (D3D11).
	///
	int shared_texture_enabled;

	///
	/// Set to true (1) to enable the ability to issue BeginFrame requests from
	/// the client application by calling CefBrowserHost::SendExternalBeginFrame.
	///
	int external_begin_frame_enabled;

	///
	/// Handle for the new browser window. Only used with windowed rendering.
	///
	cef_window_handle_t window;
} cef_window_info_t;

int (*cef_initialize)(const cef_main_args_t* args,
	const struct _cef_settings_t* settings,
	struct _cef_app_t* application,
	void* windows_sandbox_info);

void (*cef_shutdown)();

///
/// Implement this structure to provide handler implementations.
///
typedef struct _cef_client_t {
	///
	/// Base structure.
	///
	cef_base_ref_counted_t base;

	///
	/// Return the handler for audio rendering events.
	///
	struct _cef_audio_handler_t* (CEF_CALLBACK* get_audio_handler)(
		struct _cef_client_t* self);

	///
	/// Return the handler for commands. If no handler is provided the default
	/// implementation will be used.
	///
	struct _cef_command_handler_t* (CEF_CALLBACK* get_command_handler)(
		struct _cef_client_t* self);

	///
	/// Return the handler for context menus. If no handler is provided the
	/// default implementation will be used.
	///
	struct _cef_context_menu_handler_t* (CEF_CALLBACK* get_context_menu_handler)(
		struct _cef_client_t* self);

	///
	/// Return the handler for dialogs. If no handler is provided the default
	/// implementation will be used.
	///
	struct _cef_dialog_handler_t* (CEF_CALLBACK* get_dialog_handler)(
		struct _cef_client_t* self);

	///
	/// Return the handler for browser display state events.
	///
	struct _cef_display_handler_t* (CEF_CALLBACK* get_display_handler)(
		struct _cef_client_t* self);

	///
	/// Return the handler for download events. If no handler is returned
	/// downloads will not be allowed.
	///
	struct _cef_download_handler_t* (CEF_CALLBACK* get_download_handler)(
		struct _cef_client_t* self);

	///
	/// Return the handler for drag events.
	///
	struct _cef_drag_handler_t* (CEF_CALLBACK* get_drag_handler)(
		struct _cef_client_t* self);

	///
	/// Return the handler for find result events.
	///
	struct _cef_find_handler_t* (CEF_CALLBACK* get_find_handler)(
		struct _cef_client_t* self);

	///
	/// Return the handler for focus events.
	///
	struct _cef_focus_handler_t* (CEF_CALLBACK* get_focus_handler)(
		struct _cef_client_t* self);

	///
	/// Return the handler for events related to cef_frame_t lifespan. This
	/// function will be called once during cef_browser_t creation and the result
	/// will be cached for performance reasons.
	///
	struct _cef_frame_handler_t* (CEF_CALLBACK* get_frame_handler)(
		struct _cef_client_t* self);

	///
	/// Return the handler for permission requests.
	///
	struct _cef_permission_handler_t* (CEF_CALLBACK* get_permission_handler)(
		struct _cef_client_t* self);

	///
	/// Return the handler for JavaScript dialogs. If no handler is provided the
	/// default implementation will be used.
	///
	struct _cef_jsdialog_handler_t* (CEF_CALLBACK* get_jsdialog_handler)(
		struct _cef_client_t* self);

	///
	/// Return the handler for keyboard events.
	///
	struct _cef_keyboard_handler_t* (CEF_CALLBACK* get_keyboard_handler)(
		struct _cef_client_t* self);

	///
	/// Return the handler for browser life span events.
	///
	struct _cef_life_span_handler_t* (CEF_CALLBACK* get_life_span_handler)(
		struct _cef_client_t* self);

	///
	/// Return the handler for browser load status events.
	///
	struct _cef_load_handler_t* (CEF_CALLBACK* get_load_handler)(
		struct _cef_client_t* self);

	///
	/// Return the handler for printing on Linux. If a print handler is not
	/// provided then printing will not be supported on the Linux platform.
	///
	struct _cef_print_handler_t* (CEF_CALLBACK* get_print_handler)(
		struct _cef_client_t* self);

	///
	/// Return the handler for off-screen rendering events.
	///
	struct _cef_render_handler_t* (CEF_CALLBACK* get_render_handler)(
		struct _cef_client_t* self);

	///
	/// Return the handler for browser request events.
	///
	struct _cef_request_handler_t* (CEF_CALLBACK* get_request_handler)(
		struct _cef_client_t* self);

	///
	/// Called when a new message is received from a different process. Return
	/// true (1) if the message was handled or false (0) otherwise.  It is safe to
	/// keep a reference to |message| outside of this callback.
	///
	int(CEF_CALLBACK* on_process_message_received)(
		struct _cef_client_t* self,
		struct _cef_browser_t* browser,
		struct _cef_frame_t* frame,
		cef_process_id_t source_process,
		struct _cef_process_message_t* message);
} cef_client_t;

///
/// Structure representing a dictionary value. Can be used on any process and
/// thread.
///
typedef struct _cef_dictionary_value_t {
	///
	/// Base structure.
	///
	cef_base_ref_counted_t base;

	///
	/// Returns true (1) if this object is valid. This object may become invalid
	/// if the underlying data is owned by another object (e.g. list or
	/// dictionary) and that other object is then modified or destroyed. Do not
	/// call any other functions if this function returns false (0).
	///
	int(CEF_CALLBACK* is_valid)(struct _cef_dictionary_value_t* self);

	///
	/// Returns true (1) if this object is currently owned by another object.
	///
	int(CEF_CALLBACK* is_owned)(struct _cef_dictionary_value_t* self);

	///
	/// Returns true (1) if the values of this object are read-only. Some APIs may
	/// expose read-only objects.
	///
	int(CEF_CALLBACK* is_read_only)(struct _cef_dictionary_value_t* self);

	///
	/// Returns true (1) if this object and |that| object have the same underlying
	/// data. If true (1) modifications to this object will also affect |that|
	/// object and vice-versa.
	///
	int(CEF_CALLBACK* is_same)(struct _cef_dictionary_value_t* self,
		struct _cef_dictionary_value_t* that);

	///
	/// Returns true (1) if this object and |that| object have an equivalent
	/// underlying value but are not necessarily the same object.
	///
	int(CEF_CALLBACK* is_equal)(struct _cef_dictionary_value_t* self,
		struct _cef_dictionary_value_t* that);

	///
	/// Returns a writable copy of this object. If |exclude_NULL_children| is true
	/// (1) any NULL dictionaries or lists will be excluded from the copy.
	///
	struct _cef_dictionary_value_t* (CEF_CALLBACK* copy)(
		struct _cef_dictionary_value_t* self,
		int exclude_empty_children);

	///
	/// Returns the number of values.
	///
	size_t(CEF_CALLBACK* get_size)(struct _cef_dictionary_value_t* self);

	///
	/// Removes all values. Returns true (1) on success.
	///
	int(CEF_CALLBACK* clear)(struct _cef_dictionary_value_t* self);

	///
	/// Returns true (1) if the current dictionary has a value for the given key.
	///
	int(CEF_CALLBACK* has_key)(struct _cef_dictionary_value_t* self,
		const cef_string_t* key);

	///
	/// Reads all keys for this dictionary into the specified vector.
	///
	int(CEF_CALLBACK* get_keys)(struct _cef_dictionary_value_t* self,
		cef_string_list_t keys);

	///
	/// Removes the value at the specified key. Returns true (1) is the value was
	/// removed successfully.
	///
	int(CEF_CALLBACK* remove)(struct _cef_dictionary_value_t* self,
		const cef_string_t* key);

	///
	/// Returns the value type for the specified key.
	///
	cef_value_type_t(CEF_CALLBACK* get_type)(struct _cef_dictionary_value_t* self,
		const cef_string_t* key);

	///
	/// Returns the value at the specified key. For simple types the returned
	/// value will copy existing data and modifications to the value will not
	/// modify this object. For complex types (binary, dictionary and list) the
	/// returned value will reference existing data and modifications to the value
	/// will modify this object.
	///
	struct _cef_value_t* (CEF_CALLBACK* get_value)(
		struct _cef_dictionary_value_t* self,
		const cef_string_t* key);

	///
	/// Returns the value at the specified key as type bool.
	///
	int(CEF_CALLBACK* get_bool)(struct _cef_dictionary_value_t* self,
		const cef_string_t* key);

	///
	/// Returns the value at the specified key as type int.
	///
	int(CEF_CALLBACK* get_int)(struct _cef_dictionary_value_t* self,
		const cef_string_t* key);

	///
	/// Returns the value at the specified key as type double.
	///
	double(CEF_CALLBACK* get_double)(struct _cef_dictionary_value_t* self,
		const cef_string_t* key);

	///
	/// Returns the value at the specified key as type string.
	///
	// The resulting string must be freed by calling cef_string_userfree_free().
	cef_string_userfree_t(CEF_CALLBACK* get_string)(
		struct _cef_dictionary_value_t* self,
		const cef_string_t* key);

	///
	/// Returns the value at the specified key as type binary. The returned value
	/// will reference existing data.
	///
	struct _cef_binary_value_t* (CEF_CALLBACK* get_binary)(
		struct _cef_dictionary_value_t* self,
		const cef_string_t* key);

	///
	/// Returns the value at the specified key as type dictionary. The returned
	/// value will reference existing data and modifications to the value will
	/// modify this object.
	///
	struct _cef_dictionary_value_t* (CEF_CALLBACK* get_dictionary)(
		struct _cef_dictionary_value_t* self,
		const cef_string_t* key);

	///
	/// Returns the value at the specified key as type list. The returned value
	/// will reference existing data and modifications to the value will modify
	/// this object.
	///
	struct _cef_list_value_t* (CEF_CALLBACK* get_list)(
		struct _cef_dictionary_value_t* self,
		const cef_string_t* key);

	///
	/// Sets the value at the specified key. Returns true (1) if the value was set
	/// successfully. If |value| represents simple data then the underlying data
	/// will be copied and modifications to |value| will not modify this object.
	/// If |value| represents complex data (binary, dictionary or list) then the
	/// underlying data will be referenced and modifications to |value| will
	/// modify this object.
	///
	int(CEF_CALLBACK* set_value)(struct _cef_dictionary_value_t* self,
		const cef_string_t* key,
		struct _cef_value_t* value);

	///
	/// Sets the value at the specified key as type null. Returns true (1) if the
	/// value was set successfully.
	///
	int(CEF_CALLBACK* set_null)(struct _cef_dictionary_value_t* self,
		const cef_string_t* key);

	///
	/// Sets the value at the specified key as type bool. Returns true (1) if the
	/// value was set successfully.
	///
	int(CEF_CALLBACK* set_bool)(struct _cef_dictionary_value_t* self,
		const cef_string_t* key,
		int value);

	///
	/// Sets the value at the specified key as type int. Returns true (1) if the
	/// value was set successfully.
	///
	int(CEF_CALLBACK* set_int)(struct _cef_dictionary_value_t* self,
		const cef_string_t* key,
		int value);

	///
	/// Sets the value at the specified key as type double. Returns true (1) if
	/// the value was set successfully.
	///
	int(CEF_CALLBACK* set_double)(struct _cef_dictionary_value_t* self,
		const cef_string_t* key,
		double value);

	///
	/// Sets the value at the specified key as type string. Returns true (1) if
	/// the value was set successfully.
	///
	int(CEF_CALLBACK* set_string)(struct _cef_dictionary_value_t* self,
		const cef_string_t* key,
		const cef_string_t* value);

	///
	/// Sets the value at the specified key as type binary. Returns true (1) if
	/// the value was set successfully. If |value| is currently owned by another
	/// object then the value will be copied and the |value| reference will not
	/// change. Otherwise, ownership will be transferred to this object and the
	/// |value| reference will be invalidated.
	///
	int(CEF_CALLBACK* set_binary)(struct _cef_dictionary_value_t* self,
		const cef_string_t* key,
		struct _cef_binary_value_t* value);

	///
	/// Sets the value at the specified key as type dict. Returns true (1) if the
	/// value was set successfully. If |value| is currently owned by another
	/// object then the value will be copied and the |value| reference will not
	/// change. Otherwise, ownership will be transferred to this object and the
	/// |value| reference will be invalidated.
	///
	int(CEF_CALLBACK* set_dictionary)(struct _cef_dictionary_value_t* self,
		const cef_string_t* key,
		struct _cef_dictionary_value_t* value);

	///
	/// Sets the value at the specified key as type list. Returns true (1) if the
	/// value was set successfully. If |value| is currently owned by another
	/// object then the value will be copied and the |value| reference will not
	/// change. Otherwise, ownership will be transferred to this object and the
	/// |value| reference will be invalidated.
	///
	int(CEF_CALLBACK* set_list)(struct _cef_dictionary_value_t* self,
		const cef_string_t* key,
		struct _cef_list_value_t* value);
} cef_dictionary_value_t;

///
/// Manage access to preferences.
///
typedef struct _cef_preference_manager_t {
	///
	/// Base structure.
	///
	cef_base_ref_counted_t base;

	///
	/// Returns true (1) if a preference with the specified |name| exists. This
	/// function must be called on the browser process UI thread.
	///
	int(CEF_CALLBACK* has_preference)(struct _cef_preference_manager_t* self,
		const cef_string_t* name);

	///
	/// Returns the value for the preference with the specified |name|. Returns
	/// NULL if the preference does not exist. The returned object contains a copy
	/// of the underlying preference value and modifications to the returned
	/// object will not modify the underlying preference value. This function must
	/// be called on the browser process UI thread.
	///
	struct _cef_value_t* (CEF_CALLBACK* get_preference)(
		struct _cef_preference_manager_t* self,
		const cef_string_t* name);

	///
	/// Returns all preferences as a dictionary. If |include_defaults| is true (1)
	/// then preferences currently at their default value will be included. The
	/// returned object contains a copy of the underlying preference values and
	/// modifications to the returned object will not modify the underlying
	/// preference values. This function must be called on the browser process UI
	/// thread.
	///
	struct _cef_dictionary_value_t* (CEF_CALLBACK* get_all_preferences)(
		struct _cef_preference_manager_t* self,
		int include_defaults);

	///
	/// Returns true (1) if the preference with the specified |name| can be
	/// modified using SetPreference. As one example preferences set via the
	/// command-line usually cannot be modified. This function must be called on
	/// the browser process UI thread.
	///
	int(CEF_CALLBACK* can_set_preference)(struct _cef_preference_manager_t* self,
		const cef_string_t* name);

	///
	/// Set the |value| associated with preference |name|. Returns true (1) if the
	/// value is set successfully and false (0) otherwise. If |value| is NULL the
	/// preference will be restored to its default value. If setting the
	/// preference fails then |error| will be populated with a detailed
	/// description of the problem. This function must be called on the browser
	/// process UI thread.
	///
	int(CEF_CALLBACK* set_preference)(struct _cef_preference_manager_t* self,
		const cef_string_t* name,
		struct _cef_value_t* value,
		cef_string_t* error);
} cef_preference_manager_t;

///
/// Generic callback structure used for asynchronous completion.
///
typedef struct _cef_completion_callback_t {
	///
	/// Base structure.
	///
	cef_base_ref_counted_t base;

	///
	/// Method that will be called once the task is complete.
	///
	void(CEF_CALLBACK* on_complete)(struct _cef_completion_callback_t* self);
} cef_completion_callback_t;

///
/// Represents a wall clock time in UTC. Values are not guaranteed to be
/// monotonically non-decreasing and are subject to large amounts of skew.
/// Time is stored internally as microseconds since the Windows epoch (1601).
///
/// This is equivalent of Chromium `base::Time` (see base/time/time.h).
///
typedef struct _cef_basetime_t {
	int64_t val;
} cef_basetime_t;

///
/// Cookie priority values.
///
typedef enum {
	CEF_COOKIE_PRIORITY_LOW = -1,
	CEF_COOKIE_PRIORITY_MEDIUM = 0,
	CEF_COOKIE_PRIORITY_HIGH = 1,
} cef_cookie_priority_t;

///
/// Cookie same site values.
///
typedef enum {
	CEF_COOKIE_SAME_SITE_UNSPECIFIED,
	CEF_COOKIE_SAME_SITE_NO_RESTRICTION,
	CEF_COOKIE_SAME_SITE_LAX_MODE,
	CEF_COOKIE_SAME_SITE_STRICT_MODE,
} cef_cookie_same_site_t;

///
/// Cookie information.
///
typedef struct _cef_cookie_t {
	///
	/// The cookie name.
	///
	cef_string_t name;

	///
	/// The cookie value.
	///
	cef_string_t value;

	///
	/// If |domain| is empty a host cookie will be created instead of a domain
	/// cookie. Domain cookies are stored with a leading "." and are visible to
	/// sub-domains whereas host cookies are not.
	///
	cef_string_t domain;

	///
	/// If |path| is non-empty only URLs at or below the path will get the cookie
	/// value.
	///
	cef_string_t path;

	///
	/// If |secure| is true the cookie will only be sent for HTTPS requests.
	///
	int secure;

	///
	/// If |httponly| is true the cookie will only be sent for HTTP requests.
	///
	int httponly;

	///
	/// The cookie creation date. This is automatically populated by the system on
	/// cookie creation.
	///
	cef_basetime_t creation;

	///
	/// The cookie last access date. This is automatically populated by the system
	/// on access.
	///
	cef_basetime_t last_access;

	///
	/// The cookie expiration date is only valid if |has_expires| is true.
	///
	int has_expires;
	cef_basetime_t expires;

	///
	/// Same site.
	///
	cef_cookie_same_site_t same_site;

	///
	/// Priority.
	///
	cef_cookie_priority_t priority;
} cef_cookie_t;

///
/// Structure used for managing cookies. The functions of this structure may be
/// called on any thread unless otherwise indicated.
///
typedef struct _cef_cookie_manager_t {
	///
	/// Base structure.
	///
	cef_base_ref_counted_t base;

	///
	/// Visit all cookies on the UI thread. The returned cookies are ordered by
	/// longest path, then by earliest creation date. Returns false (0) if cookies
	/// cannot be accessed.
	///
	int(CEF_CALLBACK* visit_all_cookies)(struct _cef_cookie_manager_t* self,
		struct _cef_cookie_visitor_t* visitor);

	///
	/// Visit a subset of cookies on the UI thread. The results are filtered by
	/// the given url scheme, host, domain and path. If |includeHttpOnly| is true
	/// (1) HTTP-only cookies will also be included in the results. The returned
	/// cookies are ordered by longest path, then by earliest creation date.
	/// Returns false (0) if cookies cannot be accessed.
	///
	int(CEF_CALLBACK* visit_url_cookies)(struct _cef_cookie_manager_t* self,
		const cef_string_t* url,
		int includeHttpOnly,
		struct _cef_cookie_visitor_t* visitor);

	///
	/// Sets a cookie given a valid URL and explicit user-provided cookie
	/// attributes. This function expects each attribute to be well-formed. It
	/// will check for disallowed characters (e.g. the ';' character is disallowed
	/// within the cookie value attribute) and fail without setting the cookie if
	/// such characters are found. If |callback| is non-NULL it will be executed
	/// asnychronously on the UI thread after the cookie has been set. Returns
	/// false (0) if an invalid URL is specified or if cookies cannot be accessed.
	///
	int(CEF_CALLBACK* set_cookie)(struct _cef_cookie_manager_t* self,
		const cef_string_t* url,
		const struct _cef_cookie_t* cookie,
		struct _cef_set_cookie_callback_t* callback);

	///
	/// Delete all cookies that match the specified parameters. If both |url| and
	/// |cookie_name| values are specified all host and domain cookies matching
	/// both will be deleted. If only |url| is specified all host cookies (but not
	/// domain cookies) irrespective of path will be deleted. If |url| is NULL all
	/// cookies for all hosts and domains will be deleted. If |callback| is non-
	/// NULL it will be executed asnychronously on the UI thread after the cookies
	/// have been deleted. Returns false (0) if a non-NULL invalid URL is
	/// specified or if cookies cannot be accessed. Cookies can alternately be
	/// deleted using the Visit*Cookies() functions.
	///
	int(CEF_CALLBACK* delete_cookies)(
		struct _cef_cookie_manager_t* self,
		const cef_string_t* url,
		const cef_string_t* cookie_name,
		struct _cef_delete_cookies_callback_t* callback);

	///
	/// Flush the backing store (if any) to disk. If |callback| is non-NULL it
	/// will be executed asnychronously on the UI thread after the flush is
	/// complete. Returns false (0) if cookies cannot be accessed.
	///
	int(CEF_CALLBACK* flush_store)(struct _cef_cookie_manager_t* self,
		struct _cef_completion_callback_t* callback);
} cef_cookie_manager_t;

///
/// Returns the global cookie manager. By default data will be stored at
/// cef_settings_t.cache_path if specified or in memory otherwise. If |callback|
/// is non-NULL it will be executed asnychronously on the UI thread after the
/// manager's storage has been initialized. Using this function is equivalent to
/// calling cef_request_context_t::cef_request_context_get_global_context()->Get
/// DefaultCookieManager().
///
cef_cookie_manager_t* (*cef_cookie_manager_get_global_manager)(
	struct _cef_completion_callback_t* callback);

///
/// Structure to implement for visiting cookie values. The functions of this
/// structure will always be called on the UI thread.
///
typedef struct _cef_cookie_visitor_t {
	///
	/// Base structure.
	///
	cef_base_ref_counted_t base;

	///
	/// Method that will be called once for each cookie. |count| is the 0-based
	/// index for the current cookie. |total| is the total number of cookies. Set
	/// |deleteCookie| to true (1) to delete the cookie currently being visited.
	/// Return false (0) to stop visiting cookies. This function may never be
	/// called if no cookies are found.
	///
	int(CEF_CALLBACK* visit)(struct _cef_cookie_visitor_t* self,
		const struct _cef_cookie_t* cookie,
		int count,
		int total,
		int* deleteCookie);
} cef_cookie_visitor_t;

///
/// Structure to implement to be notified of asynchronous completion via
/// cef_cookie_manager_t::set_cookie().
///
typedef struct _cef_set_cookie_callback_t {
	///
	/// Base structure.
	///
	cef_base_ref_counted_t base;

	///
	/// Method that will be called upon completion. |success| will be true (1) if
	/// the cookie was set successfully.
	///
	void(CEF_CALLBACK* on_complete)(struct _cef_set_cookie_callback_t* self,
		int success);
} cef_set_cookie_callback_t;

///
/// Structure to implement to be notified of asynchronous completion via
/// cef_cookie_manager_t::delete_cookies().
///
typedef struct _cef_delete_cookies_callback_t {
	///
	/// Base structure.
	///
	cef_base_ref_counted_t base;

	///
	/// Method that will be called upon completion. |num_deleted| will be the
	/// number of cookies that were deleted.
	///
	void(CEF_CALLBACK* on_complete)(struct _cef_delete_cookies_callback_t* self,
		int num_deleted);
} cef_delete_cookies_callback_t;

///
/// A request context provides request handling for a set of related browser or
/// URL request objects. A request context can be specified when creating a new
/// browser via the cef_browser_host_t static factory functions or when creating
/// a new URL request via the cef_urlrequest_t static factory functions. Browser
/// objects with different request contexts will never be hosted in the same
/// render process. Browser objects with the same request context may or may not
/// be hosted in the same render process depending on the process model. Browser
/// objects created indirectly via the JavaScript window.open function or
/// targeted links will share the same render process and the same request
/// context as the source browser. When running in single-process mode there is
/// only a single render process (the main process) and so all browsers created
/// in single-process mode will share the same request context. This will be the
/// first request context passed into a cef_browser_host_t static factory
/// function and all other request context objects will be ignored.
///
typedef struct _cef_request_context_t {
	///
	/// Base structure.
	///
	cef_preference_manager_t base;

	///
	/// Returns true (1) if this object is pointing to the same context as |that|
	/// object.
	///
	int(CEF_CALLBACK* is_same)(struct _cef_request_context_t* self,
		struct _cef_request_context_t* other);

	///
	/// Returns true (1) if this object is sharing the same storage as |that|
	/// object.
	///
	int(CEF_CALLBACK* is_sharing_with)(struct _cef_request_context_t* self,
		struct _cef_request_context_t* other);

	///
	/// Returns true (1) if this object is the global context. The global context
	/// is used by default when creating a browser or URL request with a NULL
	/// context argument.
	///
	int(CEF_CALLBACK* is_global)(struct _cef_request_context_t* self);

	///
	/// Returns the handler for this context if any.
	///
	struct _cef_request_context_handler_t* (CEF_CALLBACK* get_handler)(
		struct _cef_request_context_t* self);

	///
	/// Returns the cache path for this object. If NULL an "incognito mode" in-
	/// memory cache is being used.
	///
	// The resulting string must be freed by calling cef_string_userfree_free().
	cef_string_userfree_t(CEF_CALLBACK* get_cache_path)(
		struct _cef_request_context_t* self);

	///
	/// Returns the cookie manager for this object. If |callback| is non-NULL it
	/// will be executed asnychronously on the UI thread after the manager's
	/// storage has been initialized.
	///
	struct _cef_cookie_manager_t* (CEF_CALLBACK* get_cookie_manager)(
		struct _cef_request_context_t* self,
		struct _cef_completion_callback_t* callback);

	///
	/// Register a scheme handler factory for the specified |scheme_name| and
	/// optional |domain_name|. An NULL |domain_name| value for a standard scheme
	/// will cause the factory to match all domain names. The |domain_name| value
	/// will be ignored for non-standard schemes. If |scheme_name| is a built-in
	/// scheme and no handler is returned by |factory| then the built-in scheme
	/// handler factory will be called. If |scheme_name| is a custom scheme then
	/// you must also implement the cef_app_t::on_register_custom_schemes()
	/// function in all processes. This function may be called multiple times to
	/// change or remove the factory that matches the specified |scheme_name| and
	/// optional |domain_name|. Returns false (0) if an error occurs. This
	/// function may be called on any thread in the browser process.
	///
	int(CEF_CALLBACK* register_scheme_handler_factory)(
		struct _cef_request_context_t* self,
		const cef_string_t* scheme_name,
		const cef_string_t* domain_name,
		struct _cef_scheme_handler_factory_t* factory);

	///
	/// Clear all registered scheme handler factories. Returns false (0) on error.
	/// This function may be called on any thread in the browser process.
	///
	int(CEF_CALLBACK* clear_scheme_handler_factories)(
		struct _cef_request_context_t* self);

	///
	/// Clears all certificate exceptions that were added as part of handling
	/// cef_request_handler_t::on_certificate_error(). If you call this it is
	/// recommended that you also call close_all_connections() or you risk not
	/// being prompted again for server certificates if you reconnect quickly. If
	/// |callback| is non-NULL it will be executed on the UI thread after
	/// completion.
	///
	void(CEF_CALLBACK* clear_certificate_exceptions)(
		struct _cef_request_context_t* self,
		struct _cef_completion_callback_t* callback);

	///
	/// Clears all HTTP authentication credentials that were added as part of
	/// handling GetAuthCredentials. If |callback| is non-NULL it will be executed
	/// on the UI thread after completion.
	///
	void(CEF_CALLBACK* clear_http_auth_credentials)(
		struct _cef_request_context_t* self,
		struct _cef_completion_callback_t* callback);

	///
	/// Clears all active and idle connections that Chromium currently has. This
	/// is only recommended if you have released all other CEF objects but don't
	/// yet want to call cef_shutdown(). If |callback| is non-NULL it will be
	/// executed on the UI thread after completion.
	///
	void(CEF_CALLBACK* close_all_connections)(
		struct _cef_request_context_t* self,
		struct _cef_completion_callback_t* callback);

	///
	/// Attempts to resolve |origin| to a list of associated IP addresses.
	/// |callback| will be executed on the UI thread after completion.
	///
	void(CEF_CALLBACK* resolve_host)(struct _cef_request_context_t* self,
		const cef_string_t* origin,
		struct _cef_resolve_callback_t* callback);

	///
	/// Load an extension.
	///
	/// If extension resources will be read from disk using the default load
	/// implementation then |root_directory| should be the absolute path to the
	/// extension resources directory and |manifest| should be NULL. If extension
	/// resources will be provided by the client (e.g. via cef_request_handler_t
	/// and/or cef_extension_handler_t) then |root_directory| should be a path
	/// component unique to the extension (if not absolute this will be internally
	/// prefixed with the PK_DIR_RESOURCES path) and |manifest| should contain the
	/// contents that would otherwise be read from the "manifest.json" file on
	/// disk.
	///
	/// The loaded extension will be accessible in all contexts sharing the same
	/// storage (HasExtension returns true (1)). However, only the context on
	/// which this function was called is considered the loader (DidLoadExtension
	/// returns true (1)) and only the loader will receive
	/// cef_request_context_handler_t callbacks for the extension.
	///
	/// cef_extension_handler_t::OnExtensionLoaded will be called on load success
	/// or cef_extension_handler_t::OnExtensionLoadFailed will be called on load
	/// failure.
	///
	/// If the extension specifies a background script via the "background"
	/// manifest key then cef_extension_handler_t::OnBeforeBackgroundBrowser will
	/// be called to create the background browser. See that function for
	/// additional information about background scripts.
	///
	/// For visible extension views the client application should evaluate the
	/// manifest to determine the correct extension URL to load and then pass that
	/// URL to the cef_browser_host_t::CreateBrowser* function after the extension
	/// has loaded. For example, the client can look for the "browser_action"
	/// manifest key as documented at
	/// https://developer.chrome.com/extensions/browserAction. Extension URLs take
	/// the form "chrome-extension://<extension_id>/<path>".
	///
	/// Browsers that host extensions differ from normal browsers as follows:
	///  - Can access chrome.* JavaScript APIs if allowed by the manifest. Visit
	///    chrome://extensions-support for the list of extension APIs currently
	///    supported by CEF.
	///  - Main frame navigation to non-extension content is blocked.
	///  - Pinch-zooming is disabled.
	///  - CefBrowserHost::GetExtension returns the hosted extension.
	///  - CefBrowserHost::IsBackgroundHost returns true for background hosts.
	///
	/// See https://developer.chrome.com/extensions for extension implementation
	/// and usage documentation.
	///
	void(CEF_CALLBACK* load_extension)(struct _cef_request_context_t* self,
		const cef_string_t* root_directory,
		struct _cef_dictionary_value_t* manifest,
		struct _cef_extension_handler_t* handler);

	///
	/// Returns true (1) if this context was used to load the extension identified
	/// by |extension_id|. Other contexts sharing the same storage will also have
	/// access to the extension (see HasExtension). This function must be called
	/// on the browser process UI thread.
	///
	int(CEF_CALLBACK* did_load_extension)(struct _cef_request_context_t* self,
		const cef_string_t* extension_id);

	///
	/// Returns true (1) if this context has access to the extension identified by
	/// |extension_id|. This may not be the context that was used to load the
	/// extension (see DidLoadExtension). This function must be called on the
	/// browser process UI thread.
	///
	int(CEF_CALLBACK* has_extension)(struct _cef_request_context_t* self,
		const cef_string_t* extension_id);

	///
	/// Retrieve the list of all extensions that this context has access to (see
	/// HasExtension). |extension_ids| will be populated with the list of
	/// extension ID values. Returns true (1) on success. This function must be
	/// called on the browser process UI thread.
	///
	int(CEF_CALLBACK* get_extensions)(struct _cef_request_context_t* self,
		cef_string_list_t extension_ids);

	///
	/// Returns the extension matching |extension_id| or NULL if no matching
	/// extension is accessible in this context (see HasExtension). This function
	/// must be called on the browser process UI thread.
	///
	struct _cef_extension_t* (CEF_CALLBACK* get_extension)(
		struct _cef_request_context_t* self,
		const cef_string_t* extension_id);

	///
	/// Returns the MediaRouter object associated with this context.  If
	/// |callback| is non-NULL it will be executed asnychronously on the UI thread
	/// after the manager's context has been initialized.
	///
	struct _cef_media_router_t* (CEF_CALLBACK* get_media_router)(
		struct _cef_request_context_t* self,
		struct _cef_completion_callback_t* callback);

	///
	/// Returns the current value for |content_type| that applies for the
	/// specified URLs. If both URLs are NULL the default value will be returned.
	/// Returns nullptr if no value is configured. Must be called on the browser
	/// process UI thread.
	///
	struct _cef_value_t* (CEF_CALLBACK* get_website_setting)(
		struct _cef_request_context_t* self,
		const cef_string_t* requesting_url,
		const cef_string_t* top_level_url,
		cef_content_setting_types_t content_type);

	///
	/// Sets the current value for |content_type| for the specified URLs in the
	/// default scope. If both URLs are NULL, and the context is not incognito,
	/// the default value will be set. Pass nullptr for |value| to remove the
	/// default value for this content type.
	///
	/// WARNING: Incorrect usage of this function may cause instability or
	/// security issues in Chromium. Make sure that you first understand the
	/// potential impact of any changes to |content_type| by reviewing the related
	/// source code in Chromium. For example, if you plan to modify
	/// CEF_CONTENT_SETTING_TYPE_POPUPS, first review and understand the usage of
	/// ContentSettingsType::POPUPS in Chromium:
	/// https://source.chromium.org/search?q=ContentSettingsType::POPUPS
	///
	void(CEF_CALLBACK* set_website_setting)(
		struct _cef_request_context_t* self,
		const cef_string_t* requesting_url,
		const cef_string_t* top_level_url,
		cef_content_setting_types_t content_type,
		struct _cef_value_t* value);

	///
	/// Returns the current value for |content_type| that applies for the
	/// specified URLs. If both URLs are NULL the default value will be returned.
	/// Returns CEF_CONTENT_SETTING_VALUE_DEFAULT if no value is configured. Must
	/// be called on the browser process UI thread.
	///
	cef_content_setting_values_t(CEF_CALLBACK* get_content_setting)(
		struct _cef_request_context_t* self,
		const cef_string_t* requesting_url,
		const cef_string_t* top_level_url,
		cef_content_setting_types_t content_type);

	///
	/// Sets the current value for |content_type| for the specified URLs in the
	/// default scope. If both URLs are NULL, and the context is not incognito,
	/// the default value will be set. Pass CEF_CONTENT_SETTING_VALUE_DEFAULT for
	/// |value| to use the default value for this content type.
	///
	/// WARNING: Incorrect usage of this function may cause instability or
	/// security issues in Chromium. Make sure that you first understand the
	/// potential impact of any changes to |content_type| by reviewing the related
	/// source code in Chromium. For example, if you plan to modify
	/// CEF_CONTENT_SETTING_TYPE_POPUPS, first review and understand the usage of
	/// ContentSettingsType::POPUPS in Chromium:
	/// https://source.chromium.org/search?q=ContentSettingsType::POPUPS
	///
	void(CEF_CALLBACK* set_content_setting)(
		struct _cef_request_context_t* self,
		const cef_string_t* requesting_url,
		const cef_string_t* top_level_url,
		cef_content_setting_types_t content_type,
		cef_content_setting_values_t value);
} cef_request_context_t;

///
/// Implement this structure to handle view events. All size and position values
/// are in density independent pixels (DIP) unless otherwise indicated. The
/// functions of this structure will be called on the browser process UI thread
/// unless otherwise indicated.
///
typedef struct _cef_view_delegate_t {
	///
	/// Base structure.
	///
	cef_base_ref_counted_t base;

	///
	/// Return the preferred size for |view|. The Layout will use this information
	/// to determine the display size.
	///
	cef_size_t(CEF_CALLBACK* get_preferred_size)(
		struct _cef_view_delegate_t* self,
		struct _cef_view_t* view);

	///
	/// Return the minimum size for |view|.
	///
	cef_size_t(CEF_CALLBACK* get_minimum_size)(struct _cef_view_delegate_t* self,
		struct _cef_view_t* view);

	///
	/// Return the maximum size for |view|.
	///
	cef_size_t(CEF_CALLBACK* get_maximum_size)(struct _cef_view_delegate_t* self,
		struct _cef_view_t* view);

	///
	/// Return the height necessary to display |view| with the provided |width|.
	/// If not specified the result of get_preferred_size().height will be used by
	/// default. Override if |view|'s preferred height depends upon the width (for
	/// example, with Labels).
	///
	int(CEF_CALLBACK* get_height_for_width)(struct _cef_view_delegate_t* self,
		struct _cef_view_t* view,
		int width);

	///
	/// Called when the parent of |view| has changed. If |view| is being added to
	/// |parent| then |added| will be true (1). If |view| is being removed from
	/// |parent| then |added| will be false (0). If |view| is being reparented the
	/// remove notification will be sent before the add notification. Do not
	/// modify the view hierarchy in this callback.
	///
	void(CEF_CALLBACK* on_parent_view_changed)(struct _cef_view_delegate_t* self,
		struct _cef_view_t* view,
		int added,
		struct _cef_view_t* parent);

	///
	/// Called when a child of |view| has changed. If |child| is being added to
	/// |view| then |added| will be true (1). If |child| is being removed from
	/// |view| then |added| will be false (0). If |child| is being reparented the
	/// remove notification will be sent to the old parent before the add
	/// notification is sent to the new parent. Do not modify the view hierarchy
	/// in this callback.
	///
	void(CEF_CALLBACK* on_child_view_changed)(struct _cef_view_delegate_t* self,
		struct _cef_view_t* view,
		int added,
		struct _cef_view_t* child);

	///
	/// Called when |view| is added or removed from the cef_window_t.
	///
	void(CEF_CALLBACK* on_window_changed)(struct _cef_view_delegate_t* self,
		struct _cef_view_t* view,
		int added);

	///
	/// Called when the layout of |view| has changed.
	///
	void(CEF_CALLBACK* on_layout_changed)(struct _cef_view_delegate_t* self,
		struct _cef_view_t* view,
		const cef_rect_t* new_bounds);

	///
	/// Called when |view| gains focus.
	///
	void(CEF_CALLBACK* on_focus)(struct _cef_view_delegate_t* self,
		struct _cef_view_t* view);

	///
	/// Called when |view| loses focus.
	///
	void(CEF_CALLBACK* on_blur)(struct _cef_view_delegate_t* self,
		struct _cef_view_t* view);
} cef_view_delegate_t;

///
/// Implement this structure to handle BrowserView events. The functions of this
/// structure will be called on the browser process UI thread unless otherwise
/// indicated.
///
typedef struct _cef_browser_view_delegate_t {
	///
	/// Base structure.
	///
	cef_view_delegate_t base;

	///
	/// Called when |browser| associated with |browser_view| is created. This
	/// function will be called after cef_life_span_handler_t::on_after_created()
	/// is called for |browser| and before on_popup_browser_view_created() is
	/// called for |browser|'s parent delegate if |browser| is a popup.
	///
	void(CEF_CALLBACK* on_browser_created)(
		struct _cef_browser_view_delegate_t* self,
		struct _cef_browser_view_t* browser_view,
		struct _cef_browser_t* browser);

	///
	/// Called when |browser| associated with |browser_view| is destroyed. Release
	/// all references to |browser| and do not attempt to execute any functions on
	/// |browser| after this callback returns. This function will be called before
	/// cef_life_span_handler_t::on_before_close() is called for |browser|.
	///
	void(CEF_CALLBACK* on_browser_destroyed)(
		struct _cef_browser_view_delegate_t* self,
		struct _cef_browser_view_t* browser_view,
		struct _cef_browser_t* browser);

	///
	/// Called before a new popup BrowserView is created. The popup originated
	/// from |browser_view|. |settings| and |client| are the values returned from
	/// cef_life_span_handler_t::on_before_popup(). |is_devtools| will be true (1)
	/// if the popup will be a DevTools browser. Return the delegate that will be
	/// used for the new popup BrowserView.
	///
	struct _cef_browser_view_delegate_t* (
		CEF_CALLBACK* get_delegate_for_popup_browser_view)(
			struct _cef_browser_view_delegate_t* self,
			struct _cef_browser_view_t* browser_view,
			const struct _cef_browser_settings_t* settings,
			struct _cef_client_t* client,
			int is_devtools);

	///
	/// Called after |popup_browser_view| is created. This function will be called
	/// after cef_life_span_handler_t::on_after_created() and on_browser_created()
	/// are called for the new popup browser. The popup originated from
	/// |browser_view|. |is_devtools| will be true (1) if the popup is a DevTools
	/// browser. Optionally add |popup_browser_view| to the views hierarchy
	/// yourself and return true (1). Otherwise return false (0) and a default
	/// cef_window_t will be created for the popup.
	///
	int(CEF_CALLBACK* on_popup_browser_view_created)(
		struct _cef_browser_view_delegate_t* self,
		struct _cef_browser_view_t* browser_view,
		struct _cef_browser_view_t* popup_browser_view,
		int is_devtools);

	///
	/// Returns the Chrome toolbar type that will be available via
	/// cef_browser_view_t::get_chrome_toolbar(). See that function for related
	/// documentation.
	///
	cef_chrome_toolbar_type_t(CEF_CALLBACK* get_chrome_toolbar_type)(
		struct _cef_browser_view_delegate_t* self);

	///
	/// Called when |browser_view| receives a gesture command. Return true (1) to
	/// handle (or disable) a |gesture_command| or false (0) to propagate the
	/// gesture to the browser for default handling. This function will only be
	/// called with the Alloy runtime. To handle these commands with the Chrome
	/// runtime implement cef_command_handler_t::OnChromeCommand instead.
	///
	int(CEF_CALLBACK* on_gesture_command)(
		struct _cef_browser_view_delegate_t* self,
		struct _cef_browser_view_t* browser_view,
		cef_gesture_command_t gesture_command);
} cef_browser_view_delegate_t;

///
/// A View is a rectangle within the views View hierarchy. It is the base
/// structure for all Views. All size and position values are in density
/// independent pixels (DIP) unless otherwise indicated. Methods must be called
/// on the browser process UI thread unless otherwise indicated.
///
typedef struct _cef_view_t {
	///
	/// Base structure.
	///
	cef_base_ref_counted_t base;

	///
	/// Returns this View as a BrowserView or NULL if this is not a BrowserView.
	///
	struct _cef_browser_view_t* (CEF_CALLBACK* as_browser_view)(
		struct _cef_view_t* self);

	///
	/// Returns this View as a Button or NULL if this is not a Button.
	///
	struct _cef_button_t* (CEF_CALLBACK* as_button)(struct _cef_view_t* self);

	///
	/// Returns this View as a Panel or NULL if this is not a Panel.
	///
	struct _cef_panel_t* (CEF_CALLBACK* as_panel)(struct _cef_view_t* self);

	///
	/// Returns this View as a ScrollView or NULL if this is not a ScrollView.
	///
	struct _cef_scroll_view_t* (CEF_CALLBACK* as_scroll_view)(
		struct _cef_view_t* self);

	///
	/// Returns this View as a Textfield or NULL if this is not a Textfield.
	///
	struct _cef_textfield_t* (CEF_CALLBACK* as_textfield)(
		struct _cef_view_t* self);

	///
	/// Returns the type of this View as a string. Used primarily for testing
	/// purposes.
	///
	// The resulting string must be freed by calling cef_string_userfree_free().
	cef_string_userfree_t(CEF_CALLBACK* get_type_string)(
		struct _cef_view_t* self);

	///
	/// Returns a string representation of this View which includes the type and
	/// various type-specific identifying attributes. If |include_children| is
	/// true (1) any child Views will also be included. Used primarily for testing
	/// purposes.
	///
	// The resulting string must be freed by calling cef_string_userfree_free().
	cef_string_userfree_t(CEF_CALLBACK* to_string)(struct _cef_view_t* self,
		int include_children);

	///
	/// Returns true (1) if this View is valid.
	///
	int(CEF_CALLBACK* is_valid)(struct _cef_view_t* self);

	///
	/// Returns true (1) if this View is currently attached to another View. A
	/// View can only be attached to one View at a time.
	///
	int(CEF_CALLBACK* is_attached)(struct _cef_view_t* self);

	///
	/// Returns true (1) if this View is the same as |that| View.
	///
	int(CEF_CALLBACK* is_same)(struct _cef_view_t* self,
		struct _cef_view_t* that);

	///
	/// Returns the delegate associated with this View, if any.
	///
	struct _cef_view_delegate_t* (CEF_CALLBACK* get_delegate)(
		struct _cef_view_t* self);

	///
	/// Returns the top-level Window hosting this View, if any.
	///
	struct _cef_window_t* (CEF_CALLBACK* get_window)(struct _cef_view_t* self);

	///
	/// Returns the ID for this View.
	///
	int(CEF_CALLBACK* get_id)(struct _cef_view_t* self);

	///
	/// Sets the ID for this View. ID should be unique within the subtree that you
	/// intend to search for it. 0 is the default ID for views.
	///
	void(CEF_CALLBACK* set_id)(struct _cef_view_t* self, int id);

	///
	/// Returns the group id of this View, or -1 if not set.
	///
	int(CEF_CALLBACK* get_group_id)(struct _cef_view_t* self);

	///
	/// A group id is used to tag Views which are part of the same logical group.
	/// Focus can be moved between views with the same group using the arrow keys.
	/// The group id is immutable once it's set.
	///
	void(CEF_CALLBACK* set_group_id)(struct _cef_view_t* self, int group_id);

	///
	/// Returns the View that contains this View, if any.
	///
	struct _cef_view_t* (CEF_CALLBACK* get_parent_view)(struct _cef_view_t* self);

	///
	/// Recursively descends the view tree starting at this View, and returns the
	/// first child that it encounters with the given ID. Returns NULL if no
	/// matching child view is found.
	///
	struct _cef_view_t* (CEF_CALLBACK* get_view_for_id)(struct _cef_view_t* self,
		int id);

	///
	/// Sets the bounds (size and position) of this View. |bounds| is in parent
	/// coordinates, or DIP screen coordinates if there is no parent.
	///
	void(CEF_CALLBACK* set_bounds)(struct _cef_view_t* self,
		const cef_rect_t* bounds);

	///
	/// Returns the bounds (size and position) of this View in parent coordinates,
	/// or DIP screen coordinates if there is no parent.
	///
	cef_rect_t(CEF_CALLBACK* get_bounds)(struct _cef_view_t* self);

	///
	/// Returns the bounds (size and position) of this View in DIP screen
	/// coordinates.
	///
	cef_rect_t(CEF_CALLBACK* get_bounds_in_screen)(struct _cef_view_t* self);

	///
	/// Sets the size of this View without changing the position. |size| in parent
	/// coordinates, or DIP screen coordinates if there is no parent.
	///
	void(CEF_CALLBACK* set_size)(struct _cef_view_t* self,
		const cef_size_t* size);

	///
	/// Returns the size of this View in parent coordinates, or DIP screen
	/// coordinates if there is no parent.
	///
	cef_size_t(CEF_CALLBACK* get_size)(struct _cef_view_t* self);

	///
	/// Sets the position of this View without changing the size. |position| is in
	/// parent coordinates, or DIP screen coordinates if there is no parent.
	///
	void(CEF_CALLBACK* set_position)(struct _cef_view_t* self,
		const cef_point_t* position);

	///
	/// Returns the position of this View. Position is in parent coordinates, or
	/// DIP screen coordinates if there is no parent.
	///
	cef_point_t(CEF_CALLBACK* get_position)(struct _cef_view_t* self);

	///
	/// Sets the insets for this View. |insets| is in parent coordinates, or DIP
	/// screen coordinates if there is no parent.
	///
	void(CEF_CALLBACK* set_insets)(struct _cef_view_t* self,
		const cef_insets_t* insets);

	///
	/// Returns the insets for this View in parent coordinates, or DIP screen
	/// coordinates if there is no parent.
	///
	cef_insets_t(CEF_CALLBACK* get_insets)(struct _cef_view_t* self);

	///
	/// Returns the size this View would like to be if enough space is available.
	/// Size is in parent coordinates, or DIP screen coordinates if there is no
	/// parent.
	///
	cef_size_t(CEF_CALLBACK* get_preferred_size)(struct _cef_view_t* self);

	///
	/// Size this View to its preferred size. Size is in parent coordinates, or
	/// DIP screen coordinates if there is no parent.
	///
	void(CEF_CALLBACK* size_to_preferred_size)(struct _cef_view_t* self);

	///
	/// Returns the minimum size for this View. Size is in parent coordinates, or
	/// DIP screen coordinates if there is no parent.
	///
	cef_size_t(CEF_CALLBACK* get_minimum_size)(struct _cef_view_t* self);

	///
	/// Returns the maximum size for this View. Size is in parent coordinates, or
	/// DIP screen coordinates if there is no parent.
	///
	cef_size_t(CEF_CALLBACK* get_maximum_size)(struct _cef_view_t* self);

	///
	/// Returns the height necessary to display this View with the provided width.
	///
	int(CEF_CALLBACK* get_height_for_width)(struct _cef_view_t* self, int width);

	///
	/// Indicate that this View and all parent Views require a re-layout. This
	/// ensures the next call to layout() will propagate to this View even if the
	/// bounds of parent Views do not change.
	///
	void(CEF_CALLBACK* invalidate_layout)(struct _cef_view_t* self);

	///
	/// Sets whether this View is visible. Windows are hidden by default and other
	/// views are visible by default. This View and any parent views must be set
	/// as visible for this View to be drawn in a Window. If this View is set as
	/// hidden then it and any child views will not be drawn and, if any of those
	/// views currently have focus, then focus will also be cleared. Painting is
	/// scheduled as needed. If this View is a Window then calling this function
	/// is equivalent to calling the Window show() and hide() functions.
	///
	void(CEF_CALLBACK* set_visible)(struct _cef_view_t* self, int visible);

	///
	/// Returns whether this View is visible. A view may be visible but still not
	/// drawn in a Window if any parent views are hidden. If this View is a Window
	/// then a return value of true (1) indicates that this Window is currently
	/// visible to the user on-screen. If this View is not a Window then call
	/// is_drawn() to determine whether this View and all parent views are visible
	/// and will be drawn.
	///
	int(CEF_CALLBACK* is_visible)(struct _cef_view_t* self);

	///
	/// Returns whether this View is visible and drawn in a Window. A view is
	/// drawn if it and all parent views are visible. If this View is a Window
	/// then calling this function is equivalent to calling is_visible().
	/// Otherwise, to determine if the containing Window is visible to the user
	/// on-screen call is_visible() on the Window.
	///
	int(CEF_CALLBACK* is_drawn)(struct _cef_view_t* self);

	///
	/// Set whether this View is enabled. A disabled View does not receive
	/// keyboard or mouse inputs. If |enabled| differs from the current value the
	/// View will be repainted. Also, clears focus if the focused View is
	/// disabled.
	///
	void(CEF_CALLBACK* set_enabled)(struct _cef_view_t* self, int enabled);

	///
	/// Returns whether this View is enabled.
	///
	int(CEF_CALLBACK* is_enabled)(struct _cef_view_t* self);

	///
	/// Sets whether this View is capable of taking focus. It will clear focus if
	/// the focused View is set to be non-focusable. This is false (0) by default
	/// so that a View used as a container does not get the focus.
	///
	void(CEF_CALLBACK* set_focusable)(struct _cef_view_t* self, int focusable);

	///
	/// Returns true (1) if this View is focusable, enabled and drawn.
	///
	int(CEF_CALLBACK* is_focusable)(struct _cef_view_t* self);

	///
	/// Return whether this View is focusable when the user requires full keyboard
	/// access, even though it may not be normally focusable.
	///
	int(CEF_CALLBACK* is_accessibility_focusable)(struct _cef_view_t* self);

	///
	/// Request keyboard focus. If this View is focusable it will become the
	/// focused View.
	///
	void(CEF_CALLBACK* request_focus)(struct _cef_view_t* self);

	///
	/// Sets the background color for this View.
	///
	void(CEF_CALLBACK* set_background_color)(struct _cef_view_t* self,
		cef_color_t color);

	///
	/// Returns the background color for this View.
	///
	cef_color_t(CEF_CALLBACK* get_background_color)(struct _cef_view_t* self);

	///
	/// Convert |point| from this View's coordinate system to DIP screen
	/// coordinates. This View must belong to a Window when calling this function.
	/// Returns true (1) if the conversion is successful or false (0) otherwise.
	/// Use cef_display_t::convert_point_to_pixels() after calling this function
	/// if further conversion to display-specific pixel coordinates is desired.
	///
	int(CEF_CALLBACK* convert_point_to_screen)(struct _cef_view_t* self,
		cef_point_t* point);

	///
	/// Convert |point| to this View's coordinate system from DIP screen
	/// coordinates. This View must belong to a Window when calling this function.
	/// Returns true (1) if the conversion is successful or false (0) otherwise.
	/// Use cef_display_t::convert_point_from_pixels() before calling this
	/// function if conversion from display-specific pixel coordinates is
	/// necessary.
	///
	int(CEF_CALLBACK* convert_point_from_screen)(struct _cef_view_t* self,
		cef_point_t* point);

	///
	/// Convert |point| from this View's coordinate system to that of the Window.
	/// This View must belong to a Window when calling this function. Returns true
	/// (1) if the conversion is successful or false (0) otherwise.
	///
	int(CEF_CALLBACK* convert_point_to_window)(struct _cef_view_t* self,
		cef_point_t* point);

	///
	/// Convert |point| to this View's coordinate system from that of the Window.
	/// This View must belong to a Window when calling this function. Returns true
	/// (1) if the conversion is successful or false (0) otherwise.
	///
	int(CEF_CALLBACK* convert_point_from_window)(struct _cef_view_t* self,
		cef_point_t* point);

	///
	/// Convert |point| from this View's coordinate system to that of |view|.
	/// |view| needs to be in the same Window but not necessarily the same view
	/// hierarchy. Returns true (1) if the conversion is successful or false (0)
	/// otherwise.
	///
	int(CEF_CALLBACK* convert_point_to_view)(struct _cef_view_t* self,
		struct _cef_view_t* view,
		cef_point_t* point);

	///
	/// Convert |point| to this View's coordinate system from that |view|. |view|
	/// needs to be in the same Window but not necessarily the same view
	/// hierarchy. Returns true (1) if the conversion is successful or false (0)
	/// otherwise.
	///
	int(CEF_CALLBACK* convert_point_from_view)(struct _cef_view_t* self,
		struct _cef_view_t* view,
		cef_point_t* point);
} cef_view_t;

///
/// Create a new browser using the window parameters specified by |windowInfo|.
/// All values will be copied internally and the actual window (if any) will be
/// created on the UI thread. If |request_context| is NULL the global request
/// context will be used. This function can be called on any browser process
/// thread and will not block. The optional |extra_info| parameter provides an
/// opportunity to specify extra information specific to the created browser
/// that will be passed to cef_render_process_handler_t::on_browser_created() in
/// the render process.
///
int (*cef_browser_host_create_browser)(
	const cef_window_info_t* windowInfo,
	struct _cef_client_t* client,
	const cef_string_t* url,
	const struct _cef_browser_settings_t* settings,
	struct _cef_dictionary_value_t* extra_info,
	struct _cef_request_context_t* request_context);
