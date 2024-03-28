// Quantum Script Extension Buffer
// Copyright (c) 2016-2024 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2016-2024 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_QUANTUMSCRIPT_EXTENSION_BUFFER_VARIABLEBUFFER_HPP
#define XYO_QUANTUMSCRIPT_EXTENSION_BUFFER_VARIABLEBUFFER_HPP

#ifndef XYO_QUANTUMSCRIPT_EXTENSION_BUFFER_DEPENDENCY_HPP
#	include <XYO/QuantumScript.Extension/Buffer/Dependency.hpp>
#endif

namespace XYO::QuantumScript::Extension::Buffer {
	class VariableBuffer;
};

namespace XYO::ManagedMemory {
	template <>
	class TMemory<XYO::QuantumScript::Extension::Buffer::VariableBuffer> : public TMemoryPoolActive<XYO::QuantumScript::Extension::Buffer::VariableBuffer> {};
};

namespace XYO::QuantumScript::Extension::Buffer {

	class VariableBuffer : public Variable {
			XYO_PLATFORM_DISALLOW_COPY_ASSIGN_MOVE(VariableBuffer);
			XYO_DYNAMIC_TYPE_DEFINE(XYO_QUANTUMSCRIPT_EXTENSION_BUFFER_EXPORT, VariableBuffer);

		protected:
			XYO_QUANTUMSCRIPT_EXTENSION_BUFFER_EXPORT static const char *strTypeBuffer;

			TPointer<Variable> vSize;

		public:
			XYO::System::Buffer buffer;

			XYO_QUANTUMSCRIPT_EXTENSION_BUFFER_EXPORT VariableBuffer();

			inline void activeDestructor() {
				buffer.activeDestructor();
			};

			XYO_QUANTUMSCRIPT_EXTENSION_BUFFER_EXPORT static Variable *newVariable();
			XYO_QUANTUMSCRIPT_EXTENSION_BUFFER_EXPORT static Variable *newVariable(size_t size);
			XYO_QUANTUMSCRIPT_EXTENSION_BUFFER_EXPORT static Variable *newVariable(const uint8_t *data, size_t dataSize);
			XYO_QUANTUMSCRIPT_EXTENSION_BUFFER_EXPORT static Variable *newVariableFromString(String str_);

			XYO_QUANTUMSCRIPT_EXTENSION_BUFFER_EXPORT TPointer<Variable> getPropertyBySymbol(Symbol symbolId);
			XYO_QUANTUMSCRIPT_EXTENSION_BUFFER_EXPORT bool hasPropertyByVariable(Variable *variable);

			XYO_QUANTUMSCRIPT_EXTENSION_BUFFER_EXPORT String getVariableType();

			XYO_QUANTUMSCRIPT_EXTENSION_BUFFER_EXPORT Variable &operatorReference(Symbol symbolId);
			XYO_QUANTUMSCRIPT_EXTENSION_BUFFER_EXPORT Variable *instancePrototype();

			XYO_QUANTUMSCRIPT_EXTENSION_BUFFER_EXPORT static void initMemory();

			XYO_QUANTUMSCRIPT_EXTENSION_BUFFER_EXPORT Variable *clone(SymbolList &inSymbolList);

			XYO_QUANTUMSCRIPT_EXTENSION_BUFFER_EXPORT bool toBoolean();
			XYO_QUANTUMSCRIPT_EXTENSION_BUFFER_EXPORT Number toNumber();
			XYO_QUANTUMSCRIPT_EXTENSION_BUFFER_EXPORT String toString();
			XYO_QUANTUMSCRIPT_EXTENSION_BUFFER_EXPORT bool isString();

			XYO_QUANTUMSCRIPT_EXTENSION_BUFFER_EXPORT void resize(size_t size);
	};

};

#endif
