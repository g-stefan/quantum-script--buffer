// Quantum Script Extension Buffer
// Copyright (c) 2016-2025 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2016-2025 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_QUANTUMSCRIPT_EXTENSION_BUFFER_LIBRARY_HPP
#define XYO_QUANTUMSCRIPT_EXTENSION_BUFFER_LIBRARY_HPP

#ifndef XYO_QUANTUMSCRIPT_EXTENSION_BUFFER_DEPENDENCY_HPP
#	include <XYO/QuantumScript.Extension/Buffer/Dependency.hpp>
#endif

namespace XYO::QuantumScript::Extension::Buffer {

	XYO_QUANTUMSCRIPT_EXTENSION_BUFFER_EXPORT void initExecutive(Executive *executive, void *extensionId);
	XYO_QUANTUMSCRIPT_EXTENSION_BUFFER_EXPORT void registerInternalExtension(Executive *executive);

};

#endif
