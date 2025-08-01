/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

// Allow use of stuff in <time.h> and abort()
#define FORBIDDEN_SYMBOL_EXCEPTION_time_h
#define FORBIDDEN_SYMBOL_EXCEPTION_abort

// Disable printf override in common/forbidden.h to avoid
// clashes with log.h from the Android SDK.
// That header file uses
//   __attribute__ ((format(printf, 3, 4)))
// which gets messed up by our override mechanism; this could
// be avoided by either changing the Android SDK to use the equally
// legal and valid
//   __attribute__ ((format(__printf__, 3, 4)))
// or by refining our printf override to use a varadic macro
// (which then wouldn't be portable, though).
// Anyway, for now we just disable the printf override globally
// for the Android port
#define FORBIDDEN_SYMBOL_EXCEPTION_printf

// Allow calling of fdopen
#define FORBIDDEN_SYMBOL_EXCEPTION_FILE

// Allow calling of close system call
#include <unistd.h>
#include <errno.h> // For remove error codes

#include "backends/platform/android/android.h"
#include "backends/platform/android/jni-android.h"

#include "backends/fs/android/android-fs-factory.h"
#include "backends/fs/android/android-saf-fs.h"

#include "backends/fs/posix/posix-iostream.h"

#include "common/debug.h"
#include "common/translation.h"
#include "common/util.h"

jclass AndroidSAFFilesystemNode::_CLS_SAFFSTree = nullptr;

jmethodID AndroidSAFFilesystemNode::_MID_addNodeRef = 0;
jmethodID AndroidSAFFilesystemNode::_MID_decNodeRef = 0;
jmethodID AndroidSAFFilesystemNode::_MID_refToNode = 0;

jmethodID AndroidSAFFilesystemNode::_MID_getTreeId = 0;
jmethodID AndroidSAFFilesystemNode::_MID_pathToNode = 0;
jmethodID AndroidSAFFilesystemNode::_MID_getChildren = 0;
jmethodID AndroidSAFFilesystemNode::_MID_getChild = 0;
jmethodID AndroidSAFFilesystemNode::_MID_createDirectory = 0;
jmethodID AndroidSAFFilesystemNode::_MID_createFile = 0;
jmethodID AndroidSAFFilesystemNode::_MID_createReadStream = 0;
jmethodID AndroidSAFFilesystemNode::_MID_createWriteStream = 0;
jmethodID AndroidSAFFilesystemNode::_MID_removeNode = 0;
jmethodID AndroidSAFFilesystemNode::_MID_removeTree = 0;

jfieldID AndroidSAFFilesystemNode::_FID__treeName = 0;
jfieldID AndroidSAFFilesystemNode::_FID__root = 0;

jmethodID AndroidSAFFilesystemNode::_MID_addRef = 0;

jfieldID AndroidSAFFilesystemNode::_FID__parent = 0;
jfieldID AndroidSAFFilesystemNode::_FID__path = 0;
jfieldID AndroidSAFFilesystemNode::_FID__documentId = 0;
jfieldID AndroidSAFFilesystemNode::_FID__flags = 0;

bool AndroidSAFFilesystemNode::_JNIinit = false;

const char AndroidSAFFilesystemNode::SAF_MOUNT_POINT[] = "/saf/";

void AndroidSAFFilesystemNode::initJNI() {
	if (_JNIinit) {
		return;
	}

	JNIEnv *env = JNI::getEnv();

	// We can't call error here as the backend is not built yet
#define FIND_STATIC_METHOD(prefix, name, signature) do {                     \
    _MID_ ## prefix ## name = env->GetStaticMethodID(cls, #name, signature); \
        if (_MID_ ## prefix ## name == 0) {                                  \
            LOGE("Can't find method ID " #name);                             \
            abort();                                                         \
        }                                                                    \
    } while (0)
#define FIND_METHOD(prefix, name, signature) do {                            \
    _MID_ ## prefix ## name = env->GetMethodID(cls, #name, signature);       \
        if (_MID_ ## prefix ## name == 0) {                                  \
            LOGE("Can't find method ID " #name);                             \
            abort();                                                         \
        }                                                                    \
    } while (0)
#define FIND_FIELD(prefix, name, signature) do {                             \
    _FID_ ## prefix ## name = env->GetFieldID(cls, #name, signature);        \
        if (_FID_ ## prefix ## name == 0) {                                  \
            LOGE("Can't find field ID " #name);                              \
            abort();                                                         \
        }                                                                    \
    } while (0)
#define SAFFSNodeSig "Lorg/scummvm/scummvm/SAFFSTree$SAFFSNode;"

	jclass cls = env->FindClass("org/scummvm/scummvm/SAFFSTree");
	_CLS_SAFFSTree = (jclass)env->NewGlobalRef(cls);

	FIND_STATIC_METHOD(, addNodeRef, "(J)V");
	FIND_STATIC_METHOD(, decNodeRef, "(J)V");
	FIND_STATIC_METHOD(, refToNode, "(J)" SAFFSNodeSig);

	FIND_METHOD(, getTreeId, "()Ljava/lang/String;");
	FIND_METHOD(, pathToNode, "(Ljava/lang/String;Z)" SAFFSNodeSig);
	FIND_METHOD(, getChildren, "(J)[" SAFFSNodeSig);
	FIND_METHOD(, getChild, "(JLjava/lang/String;)" SAFFSNodeSig);
	FIND_METHOD(, createDirectory, "(JLjava/lang/String;)" SAFFSNodeSig);
	FIND_METHOD(, createFile, "(JLjava/lang/String;)" SAFFSNodeSig);
	FIND_METHOD(, createReadStream, "(J)I");
	FIND_METHOD(, createWriteStream, "(J)I");
	FIND_METHOD(, removeNode, "(J)I");
	FIND_METHOD(, removeTree, "()V");

	FIND_FIELD(, _treeName, "Ljava/lang/String;");
	FIND_FIELD(, _root, SAFFSNodeSig);

	env->DeleteLocalRef(cls);
	cls = env->FindClass("org/scummvm/scummvm/SAFFSTree$SAFFSNode");

	FIND_METHOD(, addRef, "()J");

	FIND_FIELD(, _parent, SAFFSNodeSig);
	FIND_FIELD(, _path, "Ljava/lang/String;");
	FIND_FIELD(, _documentId, "Ljava/lang/String;");
	FIND_FIELD(, _flags, "I");

	env->DeleteLocalRef(cls);
#undef SAFFSNodeSig
#undef FIND_FIELD
#undef FIND_METHOD
#undef FIND_STATIC_METHOD

	_JNIinit = true;
}

void AndroidSAFFilesystemNode::GlobalRef::Deleter::operator()(_jobject *obj) {
			JNIEnv *env = JNI::getEnv();
			env->DeleteGlobalRef((jobject)obj);
}

void AndroidSAFFilesystemNode::NodeRef::reset() {
	if (_ref == 0) {
		return;
	}

	JNIEnv *env = JNI::getEnv();

	env->CallStaticVoidMethod(_CLS_SAFFSTree, _MID_decNodeRef, _ref);
	if (env->ExceptionCheck()) {
		LOGE("SAFFSTree::decNodeRef failed");
		env->ExceptionDescribe();
		env->ExceptionClear();
	}
	_ref = 0;
}

void AndroidSAFFilesystemNode::NodeRef::reset(const NodeRef &r) {
	if (_ref == 0 && r._ref == 0) {
		return;
	}

	JNIEnv *env = JNI::getEnv();

	if (_ref) {
		env->CallStaticVoidMethod(_CLS_SAFFSTree, _MID_decNodeRef, _ref);
		if (env->ExceptionCheck()) {
			LOGE("SAFFSTree::decNodeRef failed");
			env->ExceptionDescribe();
			env->ExceptionClear();
		}
	}

	_ref = r._ref;
	if (!_ref) {
		return;
	}

	env->CallStaticVoidMethod(_CLS_SAFFSTree, _MID_addNodeRef, _ref);
	if (env->ExceptionCheck()) {
		LOGE("SAFFSTree::addNodeRef failed");

		env->ExceptionDescribe();
		env->ExceptionClear();
		_ref = 0;
		abort();
	}
}

void AndroidSAFFilesystemNode::NodeRef::reset(JNIEnv *env, jobject node) {
	if (_ref == 0 && node == nullptr) {
		return;
	}

	if (_ref) {
		env->CallStaticVoidMethod(_CLS_SAFFSTree, _MID_decNodeRef, _ref);
		if (env->ExceptionCheck()) {
			LOGE("SAFFSTree::decNodeRef failed");
			env->ExceptionDescribe();
			env->ExceptionClear();
		}
	}

	if (node == nullptr) {
		_ref = 0;
		return;
	}

	_ref = env->CallLongMethod(node, _MID_addRef);
	if (env->ExceptionCheck()) {
		LOGE("SAFFSNode::addRef failed");

		env->ExceptionDescribe();
		env->ExceptionClear();
		_ref = 0;
		abort();
	}

	assert(_ref != 0);
}

jobject AndroidSAFFilesystemNode::NodeRef::localRef(JNIEnv *env) const {
	if (_ref == 0) {
		return nullptr;
	}

	jobject localRef = env->CallStaticObjectMethod(_CLS_SAFFSTree, _MID_refToNode, _ref);
	if (env->ExceptionCheck()) {
		LOGE("SAFFSTree::refToNode failed");

		env->ExceptionDescribe();
		env->ExceptionClear();

		return nullptr;
	}

	return localRef;
}

AndroidSAFFilesystemNode *AndroidSAFFilesystemNode::makeFromPath(const Common::String &path) {
	if (!path.hasPrefix(SAF_MOUNT_POINT)) {
		// Not a SAF mount point
		return nullptr;
	}

	// Path is in the form /saf/<treeid>/<path>
	size_t pos = path.findFirstOf('/', sizeof(SAF_MOUNT_POINT) - 1);
	Common::String treeId;
	Common::String realPath;
	if (pos == Common::String::npos) {
		treeId = path.substr(sizeof(SAF_MOUNT_POINT) - 1);
	} else {
		treeId = path.substr(sizeof(SAF_MOUNT_POINT) - 1, pos - sizeof(SAF_MOUNT_POINT) + 1);
		realPath = path.substr(pos);
	}

	jobject safTree = JNI::findSAFTree(treeId);
	if (!safTree) {
		LOGW("AndroidSAFFilesystemNode::makeFromPath: tree id %s not found", treeId.c_str());
		return nullptr;
	}

	JNIEnv *env = JNI::getEnv();

	jstring pathObj = env->NewStringUTF(realPath.c_str());

	jobject node = env->CallObjectMethod(safTree, _MID_pathToNode, pathObj, false);

	env->DeleteLocalRef(pathObj);

	if (env->ExceptionCheck()) {
		LOGE("SAFFSTree::pathToNode failed");

		env->ExceptionDescribe();
		env->ExceptionClear();

		env->DeleteLocalRef(safTree);
		return nullptr;
	}

	if (node) {
		AndroidSAFFilesystemNode *ret = new AndroidSAFFilesystemNode(GlobalRef(env, safTree), node);

		env->DeleteLocalRef(node);
		env->DeleteLocalRef(safTree);

		return ret;
	}

	// Node doesn't exist: we will try to make a node from the parent and
	// if it works we will create a non-existent node

	pos = realPath.findLastOf('/');
	if (pos == Common::String::npos || pos == 0) {
		// No / in path or at root, no parent and we have a tree: it's all good
		if (pos == 0) {
			realPath = realPath.substr(1);
		}
		AndroidSAFFilesystemNode *parent = makeFromTree(safTree);
		AndroidSAFFilesystemNode *ret = static_cast<AndroidSAFFilesystemNode *>(parent->getChild(realPath));
		delete parent;

		// safTree has already been released by makeFromTree
		return ret;
	}

	Common::String baseName(realPath.substr(pos + 1));
	realPath.erase(pos);

	pathObj = env->NewStringUTF(realPath.c_str());

	node = env->CallObjectMethod(safTree, _MID_pathToNode, pathObj, false);

	env->DeleteLocalRef(pathObj);

	if (env->ExceptionCheck()) {
		LOGE("SAFFSTree::pathToNode failed");

		env->ExceptionDescribe();
		env->ExceptionClear();

		env->DeleteLocalRef(safTree);
		return nullptr;
	}

	if (node) {
		AndroidSAFFilesystemNode *parent = new AndroidSAFFilesystemNode(GlobalRef(env, safTree), node);
		env->DeleteLocalRef(node);
		env->DeleteLocalRef(safTree);

		AndroidSAFFilesystemNode *ret = static_cast<AndroidSAFFilesystemNode *>(parent->getChild(baseName));
		delete parent;

		return ret;
	}

	env->DeleteLocalRef(safTree);
	return nullptr;
}

AndroidSAFFilesystemNode *AndroidSAFFilesystemNode::makeFromTree(jobject safTree) {
	assert(safTree);

	JNIEnv *env = JNI::getEnv();

	jobject node = env->GetObjectField(safTree, _FID__root);
	if (!node) {
		env->DeleteLocalRef(safTree);
		return nullptr;
	}

	AndroidSAFFilesystemNode *ret = new AndroidSAFFilesystemNode(GlobalRef(env, safTree), node);

	env->DeleteLocalRef(node);
	env->DeleteLocalRef(safTree);

	return ret;
}

AndroidSAFFilesystemNode::AndroidSAFFilesystemNode(const GlobalRef &safTree, jobject safNode) :
	_flags(0) {

	JNIEnv *env = JNI::getEnv();

	_safTree = safTree;
	assert(_safTree != nullptr);

	_safNode.reset(env, safNode);
	cacheData(env, safNode);
}

AndroidSAFFilesystemNode::AndroidSAFFilesystemNode(const GlobalRef &safTree, jobject safParent,
        const Common::String &path, const Common::String &name) : _flags(0) {

	JNIEnv *env = JNI::getEnv();

	_safTree = safTree;
	_safParent.reset(env, safParent);

	// In this case _path is the parent
	_path = path;
	_newName = name;
}

AndroidSAFFilesystemNode::AndroidSAFFilesystemNode(const GlobalRef &safTree,
		const NodeRef &safParent, const Common::String &path,
		const Common::String &name) : _flags(0) {

	_safTree = safTree;
	_safParent = safParent;

	// In this case _path is the parent
	_path = path;
	_newName = name;
}

Common::String AndroidSAFFilesystemNode::getName() const {
	if (!_safNode || !_safParent) {
		// _newName is for non-existent paths or root node pretty name
		return _newName;
	}

	return lastPathComponent(_path, '/');
}

Common::String AndroidSAFFilesystemNode::getPath() const {
	assert(_safTree != nullptr);

	if (_safNode) {
		return _path;
	}

	// When no node, it means _path is the parent node
	return _path + "/" + _newName;
}

AbstractFSNode *AndroidSAFFilesystemNode::getChild(const Common::String &n) const {
	assert(_safTree != nullptr);
	assert(_safNode);

	// Make sure the string contains no slashes
	assert(!n.contains('/'));

	JNIEnv *env = JNI::getEnv();

	jstring name = env->NewStringUTF(n.c_str());

	jobject child = env->CallObjectMethod(_safTree, _MID_getChild, _safNode.get(), name);

	env->DeleteLocalRef(name);

	if (env->ExceptionCheck()) {
		LOGE("SAFFSTree::getChild failed");

		env->ExceptionDescribe();
		env->ExceptionClear();

		return nullptr;
	}

	if (child) {
		AndroidSAFFilesystemNode *ret = new AndroidSAFFilesystemNode(_safTree, child);
		env->DeleteLocalRef(child);
		return ret;
	}

	return new AndroidSAFFilesystemNode(_safTree, _safNode, _path, n);
}

bool AndroidSAFFilesystemNode::getChildren(AbstractFSList &myList, ListMode mode,
        bool hidden) const {
	assert(_flags & DIRECTORY);

	assert(_safTree != nullptr);
	if (!_safNode) {
		return false;
	}

	JNIEnv *env = JNI::getEnv();

	jobjectArray array =
	    (jobjectArray)env->CallObjectMethod(_safTree, _MID_getChildren, _safNode.get());

	if (env->ExceptionCheck()) {
		LOGE("SAFFSTree::getChildren failed");

		env->ExceptionDescribe();
		env->ExceptionClear();

		return false;
	}

	if (!array) {
		// Fetching children failed: a log error has already been produced in Java code
		return false;
	}

	myList.clear();

	jsize size = env->GetArrayLength(array);
	myList.reserve(size);

	for (jsize i = 0; i < size; ++i) {
		jobject node = env->GetObjectArrayElement(array, i);

		myList.push_back(new AndroidSAFFilesystemNode(_safTree, node));

		env->DeleteLocalRef(node);
	}
	env->DeleteLocalRef(array);

	return true;
}

AbstractFSNode *AndroidSAFFilesystemNode::getParent() const {
	assert(_safTree != nullptr);
	// No need to check for _safNode: if node doesn't exist yet parent is its parent

	JNIEnv *env = JNI::getEnv();
	if (!_safParent) {
		return AndroidFilesystemFactory::instance().makeRootFileNode();
	}

	jobject parent = _safParent.localRef(env);
	assert(parent);

	AndroidSAFFilesystemNode *ret = new AndroidSAFFilesystemNode(_safTree, parent);
	env->DeleteLocalRef(parent);
	return ret;
}

Common::SeekableReadStream *AndroidSAFFilesystemNode::createReadStream() {
	assert(_safTree != nullptr);

	if (!_safNode) {
		return nullptr;
	}

	JNIEnv *env = JNI::getEnv();

	jint fd = env->CallIntMethod(_safTree, _MID_createReadStream, _safNode.get());

	if (env->ExceptionCheck()) {
		LOGE("SAFFSTree::createReadStream failed");

		env->ExceptionDescribe();
		env->ExceptionClear();

		return nullptr;
	}

	if (fd == -1) {
		return nullptr;
	}

	FILE *f = fdopen(fd, "r");
	if (!f) {
		close(fd);
		return nullptr;
	}

	return new PosixIoStream(f);
}

Common::SeekableWriteStream *AndroidSAFFilesystemNode::createWriteStream(bool atomic) {
	assert(_safTree != nullptr);

	JNIEnv *env = JNI::getEnv();

	if (!_safNode) {
		assert(_safParent);
		jstring name = env->NewStringUTF(_newName.c_str());

		// TODO: Add atomic support if possible
		jobject child = env->CallObjectMethod(_safTree, _MID_createFile, _safParent.get(), name);

		env->DeleteLocalRef(name);

		if (env->ExceptionCheck()) {
			LOGE("SAFFSTree::createFile failed");

			env->ExceptionDescribe();
			env->ExceptionClear();

			return nullptr;
		}

		if (!child) {
			return nullptr;
		}

		_safNode.reset(env, child);
		cacheData(env, child);

		env->DeleteLocalRef(child);
	}

	jint fd = env->CallIntMethod(_safTree, _MID_createWriteStream, _safNode.get());
	if (env->ExceptionCheck()) {
		LOGE("SAFFSTree::createWriteStream failed");

		env->ExceptionDescribe();
		env->ExceptionClear();

		return nullptr;
	}

	if (fd == -1) {
		return nullptr;
	}

	FILE *f = fdopen(fd, "w");
	if (!f) {
		close(fd);
		return nullptr;
	}

	return new PosixIoStream(f);
}

bool AndroidSAFFilesystemNode::createDirectory() {
	assert(_safTree != nullptr);

	if (_safNode) {
		return _flags & DIRECTORY;
	}

	assert(_safParent);

	JNIEnv *env = JNI::getEnv();

	jstring name = env->NewStringUTF(_newName.c_str());

	jobject child = env->CallObjectMethod(_safTree, _MID_createDirectory, _safParent.get(), name);

	env->DeleteLocalRef(name);

	if (env->ExceptionCheck()) {
		LOGE("SAFFSTree::createDirectory failed");

		env->ExceptionDescribe();
		env->ExceptionClear();

		return false;
	}

	if (!child) {
		return false;
	}

	_safNode.reset(env, child);

	cacheData(env, child);

	env->DeleteLocalRef(child);

	return true;
}

int AndroidSAFFilesystemNode::remove() {
	assert(_safTree != nullptr);

	if (!_safNode) {
		return ENOENT;
	}

	if (!_safParent) {
		// It's the root of the tree: we can't delete it
		return EPERM;
	}

	if (isDirectory()) {
		// Don't delete folders (yet?)
		return EPERM;
	}

	JNIEnv *env = JNI::getEnv();

	jint result = env->CallIntMethod(_safTree, _MID_removeNode, _safNode.get());

	if (env->ExceptionCheck()) {
		LOGE("SAFFSTree::removeNode failed");

		env->ExceptionDescribe();
		env->ExceptionClear();

		return EIO;
	}

	if (result) {
		return result;
	}

	_safNode.reset();

	// Create the parent node to fetch informations needed to make us a non-existent node

	jobject jparent = _safParent.localRef(env);
	if (!jparent)
		return EIO;

	AndroidSAFFilesystemNode *parent = new AndroidSAFFilesystemNode(_safTree, jparent);
	env->DeleteLocalRef(jparent);

	size_t pos = _path.findLastOf('/');
	if (pos == Common::String::npos) {
		_newName = _path;
	} else {
		_newName = _path.substr(pos + 1);
	}
	_path = parent->_path;

	delete parent;

	return 0;
}

void AndroidSAFFilesystemNode::removeTree() {
	assert(!_safParent);

	JNIEnv *env = JNI::getEnv();

	env->CallVoidMethod(_safTree, _MID_removeTree);

	if (env->ExceptionCheck()) {
		LOGE("SAFFSTree::removeTree failed");

		env->ExceptionDescribe();
		env->ExceptionClear();
	}
}

void AndroidSAFFilesystemNode::cacheData(JNIEnv *env, jobject node) {
	_flags = env->GetIntField(node, _FID__flags);

	jobject safParent = env->GetObjectField(node, _FID__parent);
	_safParent.reset(env, safParent);

	if (!_safParent) {
		jstring nameObj = (jstring)env->GetObjectField(_safTree, _FID__treeName);
		debug("_FID__treeName: %p", nameObj);
		const char *nameP = env->GetStringUTFChars(nameObj, 0);
		if (nameP != 0) {
			_newName = Common::String(nameP);
			env->ReleaseStringUTFChars(nameObj, nameP);
		}
		env->DeleteLocalRef(nameObj);
	}

	Common::String workingPath;

	jstring pathObj = (jstring)env->GetObjectField(node, _FID__path);
	debug("_FID__path: %p", pathObj);
	const char *path = env->GetStringUTFChars(pathObj, 0);
	if (path == nullptr) {
		env->DeleteLocalRef(pathObj);
		error("SAFFSNode::_path is null");
		return;
	}
	workingPath = Common::String(path);
	env->ReleaseStringUTFChars(pathObj, path);
	env->DeleteLocalRef(pathObj);

	jstring idObj = (jstring)env->CallObjectMethod(_safTree, _MID_getTreeId);
	if (env->ExceptionCheck()) {
		env->ExceptionDescribe();
		env->ExceptionClear();

		env->ReleaseStringUTFChars(pathObj, path);
		env->DeleteLocalRef(pathObj);
		error("SAFFSTree::getTreeId failed");
		return;
	}

	if (!idObj) {
		error("SAFFSTree::getTreeId returned null");
		return;
	}

	const char *id = env->GetStringUTFChars(idObj, 0);
	if (id == nullptr) {
		error("Failed to get string from SAFFSTree::getTreeId");
		env->DeleteLocalRef(idObj);
		return;
	}

	_path = Common::String(SAF_MOUNT_POINT);
	_path += id;
	_path += workingPath;
	env->ReleaseStringUTFChars(idObj, id);
	env->DeleteLocalRef(idObj);
}

const char AddSAFFakeNode::SAF_ADD_FAKE_PATH[] = "/saf";

AddSAFFakeNode::~AddSAFFakeNode() {
	delete _proxied;
}

Common::U32String AddSAFFakeNode::getDisplayName() const {
	// I18N: This is displayed in the file browser to let the user choose a new folder for Android Storage Attached Framework
	return Common::U32String::format("\x01<%s>", _("Add a new folder").c_str());
}

Common::String AddSAFFakeNode::getName() const {
	return Common::String::format("\x01<%s>", _("Add a new folder").encode().c_str());
}

AbstractFSNode *AddSAFFakeNode::getChild(const Common::String &name) const {
	if (_fromPath) {
		// When starting from /saf try to get the tree node
		return AndroidSAFFilesystemNode::makeFromPath(Common::String(AndroidSAFFilesystemNode::SAF_MOUNT_POINT) + name);
	}
	// We can't call getChild as it's protected
	return nullptr;
}

AbstractFSNode *AddSAFFakeNode::getParent() const {
	// We are always just below the root and getParent is protected
	return AndroidFilesystemFactory::instance().makeRootFileNode();
}

bool AddSAFFakeNode::exists() const {
	if (_fromPath) {
		// /saf always exists when created as a path
		return true;
	}

	if (!_proxied) {
		makeProxySAF();
	}

	if (!_proxied) {
		return false;
	}

	return _proxied->exists();
}

bool AddSAFFakeNode::getChildren(AbstractFSList &list, ListMode mode, bool hidden) const {
	if (_fromPath) {
		// When built from path, /saf lists all SAF node but never proposes to add one
		if (mode == Common::FSNode::kListFilesOnly) {
			// All directories
			return true;
		}
		AndroidFilesystemFactory::instance().getSAFTrees(list, false);
		return true;
	}

	if (!_proxied) {
		makeProxySAF();
	}

	if (!_proxied) {
		return false;
	}

	return _proxied->getChildren(list, mode, hidden);
}

Common::String AddSAFFakeNode::getPath() const {
	if (_fromPath) {
		return SAF_ADD_FAKE_PATH;
	}

	if (!_proxied) {
		makeProxySAF();
	}

	if (!_proxied) {
		return "";
	}

	return _proxied->getPath();
}

bool AddSAFFakeNode::isReadable() const {
	if (_fromPath) {
		return true;
	}

	if (!_proxied) {
		makeProxySAF();
	}

	if (!_proxied) {
		return false;
	}

	return _proxied->isReadable();
}

bool AddSAFFakeNode::isWritable() const {
	if (_fromPath) {
		return false;
	}

	if (!_proxied) {
		makeProxySAF();
	}

	if (!_proxied) {
		return false;
	}

	return _proxied->isWritable();
}

int AddSAFFakeNode::remove() {
	return EPERM;
}

void AddSAFFakeNode::makeProxySAF() const {
	assert(!_fromPath);

	if (_proxied) {
		return;
	}

	// I18N: This may be displayed in the Android UI used to add a Storage Attach Framework authorization
	jobject saftree = JNI::getNewSAFTree(true, "", _("Choose a new folder"));
	if (!saftree) {
		return;
	}

	_proxied = AndroidSAFFilesystemNode::makeFromTree(saftree);
}
