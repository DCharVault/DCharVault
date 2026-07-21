# ViewModel API Documentation

This document defines the ViewModel layer of DCharVault. Located in the `/viewmodel` directory, these classes inherit from `QObject` and act as the secure bridge between the QML frontend and the C++ Core Models. They manage UI state, handle user input securely, and emit signals to update the UI without exposing backend cryptography.

## `LoginViewModel`
Manages the authentication flow, vault creation, and system-level UI states.

### Q_PROPERTIES (Bound to QML)
*   `ClipboardSanitizer* sanitizer` (READ, CONSTANT)
    Exposes the clipboard sanitizer utility to QML.

### Q_INVOKABLE Methods (Callable from QML)
*   `void authenticate(SecurePasswordInput* passwordField, const QString& dbUrl)`
    Attempts to unlock an existing vault by passing the secure password buffer to the `DiaryManager`. Automatically resolves Android `content://` URIs by importing the database to a private sandbox.
*   `void createVault(const QString& journalName, SecurePasswordInput* passwordField, const QString& dbUrl)`
    Initializes a new encrypted vault at the specified desktop path.
*   `void createVaultAndroid(const QString& journalName, SecurePasswordInput* passwordField)`
    Initializes a new encrypted vault using Android-specific internal storage paths.
*   `void updateTitleBar(bool isDark)`
    Updates system-level window UI properties to match the active theme.

### Signals (Emitted to QML)
*   `void loginSuccess()`: Emitted when authentication or vault creation succeeds. Triggers UI navigation to the main diary view.
*   `void loginFailed()`: Emitted when authentication fails (e.g., incorrect master password or invalid MAC).
*   `void dbNotFound()`: Emitted if the target database file does not exist at the provided path.

---

## `DiaryViewModel`
Manages the state and operations for the currently active diary entry.

### Q_INVOKABLE Methods (Callable from QML)
*   `void saveNewEntry(const qint64 id, const QString& title, const QString& content)`
    Routes a save or update command to the `DiaryManager`.
*   `QString loadEntryContent(const qint64 id)`
    Requests the decrypted plaintext content of an entry. Returns the plaintext string to the UI.
*   `QString loadEntryTitle(const qint64 id)`
    Requests the decrypted plaintext title of an entry. Returns the plaintext string to the UI.
*   `void deleteEntry(const qint64 id)`
    Issues a command to permanently delete the specified entry.

### Signals (Emitted to QML)
*   `void entrySavedSuccessfully(const qint64 savedId, const QString& finalizeTitle)`: Emitted upon successful database persistence.
*   `void entrySaveFailed(const QString& errorMessage)`: Emitted if the encryption or database insertion fails.
*   `void entryDeletedSuccessfully()`: Emitted when an entry is successfully purged.
*   `void entryDeleteFailed()`: Emitted if the deletion operation fails.

---

## `DiaryListModel`
A customized `QAbstractListModel` that provides the QML views with a lightweight, iterable list of diary entries.

### Q_INVOKABLE Methods (Callable from QML)
*   `void loadEntries()`
    Commands the `DiaryManager` to decrypt all entry metadata and refreshes the internal UI list.

### Overridden C++ Methods
*   `int rowCount(const QModelIndex &parent = QModelIndex()) const`
    Returns the total number of entries currently loaded in memory.
*   `QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const`
    Maps specific data fields (Title, Date, ID) to QML delegate components.
*   `QHash<int, QByteArray> roleNames() const`
    Defines the string-based roles accessible from QML (e.g., `idRole`, `titleRole`, `dateRole`).

---

## `SessionViewModel`
Coordinates auto-locking and application session states, interfacing directly with `DiaryManager`. It provides properties for the UI to react to the application's lock state and configuration for the inactivity timeout.

### Q_PROPERTIES (Bound to QML)
*   `bool isLocked` (READ, NOTIFY)
    Indicates whether the vault is currently locked. The UI can bind to this to conditionally show the lock screen.
*   `uint32_t timeoutSeconds` (READ, WRITE, NOTIFY)
    The inactivity timeout duration in seconds.

### Q_INVOKABLE Methods (Callable from QML)
*   `void reportActivity()`
    Records user interaction in the UI to reset the inactivity timer.
*   `void lockNow()`
    Forces an immediate lock of the vault and updates the session state.
*   `void onUnlockSuccess()`
    Called when the user successfully authenticates, transitioning the session back to an active state.
*   `void setTimeoutSeconds(uint32_t seconds)`
    Updates the session inactivity timeout.

### Public Slots
*   `void onApplicationStateChanged(Qt::ApplicationState state)`
    Reacts to application background/foreground state changes to trigger locking logic.
*   `void onVaultOpened()`
    Notified when the vault is opened to initialize the session.

### Signals (Emitted to QML)
*   `void lockStateChanged()`: Emitted when the `isLocked` property changes.
*   `void sessionLocked()`: Emitted when the vault enters the locked state.
*   `void sessionUnlocked()`: Emitted when the vault returns to the active state.
*   `void timeLimitChanged()`: Emitted when the timeout duration is modified.

---

## `SecurePasswordInput`
A security-focused UI helper component. It intercepts password keystrokes and stores them directly in a memory-safe `SecureString`, preventing the master password from lingering in Qt's standard string buffers.

### Q_INVOKABLE Methods (Callable from QML)
*   `void clearPassword()`
    Securely zeroes out and clears the internal password buffer.
*   `void insertSecureByte(int byteCode)`
    Hooks for custom in-app keyboards to insert individual bytes.
*   `void insertSecureText(const QString& text)`
    Hooks for custom in-app keyboards to insert complete text sequences securely.
*   `void removeSecureByte()`
    Removes the last entered character from the secure buffer safely.
*   `void submitPassword()`
    Triggers the internal password submission workflow for auth/creation.

### Public C++ Methods
*   `int passwordLength() const`
    Returns the current length of the password (used for UI validation, like disabling the submit button if empty).
*   `const SecureString& getSecureBuffer() const`
    Provides the Core Models with a direct reference to the securely allocated password.

### Signals (Emitted to QML)
*   `void passwordLengthChanged()`: Notifies the UI when the password input length changes.
*   `void enterPressed()`: Emitted when the user presses the 'Enter' key within the input field.

---

## `ClipboardSanitizer`
A background security utility that monitors the operating system's clipboard and automatically wipes it after a predefined timeout to prevent sensitive diary data from leaking to other applications. Now configurable and persisted via `DiaryManager`.

### Q_PROPERTIES (Bound to QML)
*   `uint32_t timeoutSeconds` (READ, WRITE, NOTIFY)
    The currently active clipboard sanitization timeout duration in seconds.

### Q_INVOKABLE Methods (Callable from QML)
*   `void notifyCopied()`
    Triggered by the UI when the user copies text. Initiates the countdown timer for clipboard sanitization.
*   `void wipeNow()`
    Immediately forces a wipe of the system clipboard.
*   `void setTimeoutSeconds(uint32_t seconds)`
    Updates the active clipboard sanitization timeout and persists it to the database via `DiaryManager`.

### Public C++ Methods / Slots
*   `void onVaultOpened()`
    A slot invoked upon successful login, loading the user-configured clipboard timeout.
*   `void onSystemClipboardChanged()`
    A Qt Slot connected to the OS clipboard. It detects if the user copies new data externally and aborts the wipe timer if necessary.
*   `void executeSanitization()`
    The internal execution method that performs the actual memory overwrite of the clipboard.

### Signals (Emitted to QML)
*   `void clipboardSanitized()`: Emitted after the clipboard has been securely wiped.
*   `void externalClipboardDetected()`: Emitted when an external clipboard change is detected, interrupting the sanitization flow.

---

## `RichTextController`
A utility component that provides robust rich text capabilities, manipulating the underlying `QTextDocument` directly for advanced text editing in QML.

### Q_PROPERTIES (Bound to QML)
*   `QQuickTextDocument* textDocument` (READ, WRITE, NOTIFY)
    The text document instance to apply formatting and highlights to.

### Q_INVOKABLE Methods (Callable from QML)
*   `void setBold(int selStart, int selEnd, bool bold)`
*   `void setItalic(int selStart, int selEnd, bool italic)`
*   `void setUnderline(int selStart, int selEnd, bool underline)`
*   `void setStrikethrough(int selStart, int selEnd, bool strike)`
    Applies or removes formatting across the specified text range.
*   `void toggleBulletList(int position)`
*   `void toggleNumberedList(int position)`
*   `void toggleBlockquote(int selStart, int selEnd)`
    Toggles list blocks and blockquotes at the active cursor or selection.
*   `void setHeading(int selStart, int selEnd, int level)`
    Applies HTML header styling based on the specified heading level (1-3).
*   `void clearAllFormatting(int selStart, int selEnd)`
    Removes all inline and block text formatting.
*   `void clearBlockFormatting(int selStart, int selEnd)`
    Removes only block-level formatting for the specified selection.
*   `void setFontSize(int selStart, int selEnd, qreal size)`
*   `void setTextColor(int selStart, int selEnd, const QColor &color)`
*   `void applyHighlight(int selStart, int selEnd, const QColor &color)`
    Modifies text properties and background colors for rich text visualization.

### Signals (Emitted to QML)
*   `void textDocumentChanged()`: Emitted when the associated text document changes.