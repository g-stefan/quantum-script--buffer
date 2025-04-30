// Quantum Script Extension Buffer
// Copyright (c) 2016-2025 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2016-2025 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_QUANTUMSCRIPT_EXTENSION_BUFFER_CONTEXT_HPP
#define XYO_QUANTUMSCRIPT_EXTENSION_BUFFER_CONTEXT_HPP

#ifndef XYO_QUANTUMSCRIPT_EXTENSION_BUFFER_DEPENDENCY_HPP
#	include <XYO/QuantumScript.Extension/Buffer/Dependency.hpp>
#endif

namespace XYO::QuantumScript::Extension::Buffer {

	class BufferContext : public Object {
			XYO_PLATFORM_DISALLOW_COPY_ASSIGN_MOVE(BufferContext);

		public:
			Symbol symbolFunctionBuffer;
			TPointerX<Prototype> prototypeBuffer;

			XYO_QUANTUMSCRIPT_EXTENSION_BUFFER_EXPORT BufferContext();
	};

	XYO_QUANTUMSCRIPT_EXTENSION_BUFFER_EXPORT BufferContext *getContext();

};

#endif
