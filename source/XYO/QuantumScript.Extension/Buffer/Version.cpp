// Quantum Script Extension Buffer
// Copyright (c) 2022 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2022 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#include <XYO/QuantumScript.Extension/Buffer/Version.hpp>
#include <XYO/QuantumScript.Extension/Buffer/Version.rh>

namespace XYO::QuantumScript::Extension::Buffer::Version {

	static const char *version_ = XYO_QUANTUMSCRIPT_EXTENSION_BUFFER_VERSION_STR;
	static const char *build_ = XYO_QUANTUMSCRIPT_EXTENSION_BUFFER_VERSION_STR_BUILD;
	static const char *versionWithBuild_ = XYO_QUANTUMSCRIPT_EXTENSION_BUFFER_VERSION_STR_WITH_BUILD;
	static const char *datetime_ = XYO_QUANTUMSCRIPT_EXTENSION_BUFFER_VERSION_STR_DATETIME;

	const char *version() {
		return version_;
	};
	const char *build() {
		return build_;
	};
	const char *versionWithBuild() {
		return versionWithBuild_;
	};
	const char *datetime() {
		return datetime_;
	};

};
