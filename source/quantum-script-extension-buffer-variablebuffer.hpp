//
// Quantum Script Extension Buffer
//
// Copyright (c) 2020-2022 Grigore Stefan <g_stefan@yahoo.com>
// Created by Grigore Stefan <g_stefan@yahoo.com>
//
// MIT License (MIT) <http://opensource.org/licenses/MIT>
//

#ifndef QUANTUM_SCRIPT_VARIABLEBUFFER_HPP
#define QUANTUM_SCRIPT_VARIABLEBUFFER_HPP

#ifndef QUANTUM_SCRIPT_VARIABLE_HPP
#	include "quantum-script-variable.hpp"
#endif

#ifndef QUANTUM_SCRIPT_EXTENSION_BUFFER__EXPORT_HPP
#	include "quantum-script-extension-buffer--export.hpp"
#endif

namespace Quantum {
	namespace Script {
		namespace Extension {
			namespace Buffer {

				class VariableBuffer;

			};
		};

	};
};

namespace XYO {
	namespace ManagedMemory {
		template <>
		class TMemory<Quantum::Script::Extension::Buffer::VariableBuffer> : public TMemoryPoolActive<Quantum::Script::Extension::Buffer::VariableBuffer> {};
	};
};

namespace Quantum {
	namespace Script {
		namespace Extension {
			namespace Buffer {

				using namespace XYO;

				class VariableBuffer : public Variable {
						XYO_DISALLOW_COPY_ASSIGN_MOVE(VariableBuffer);
						XYO_DYNAMIC_TYPE_DEFINE(QUANTUM_SCRIPT_EXTENSION_BUFFER_EXPORT, VariableBuffer);

					protected:
						QUANTUM_SCRIPT_EXTENSION_BUFFER_EXPORT static const char *strTypeBuffer;

						TPointer<Variable> vSize;

					public:
						XYO::Buffer buffer;

						QUANTUM_SCRIPT_EXTENSION_BUFFER_EXPORT VariableBuffer();

						inline void activeDestructor() {
							buffer.activeDestructor();
						};

						QUANTUM_SCRIPT_EXTENSION_BUFFER_EXPORT static Variable *newVariable();
						QUANTUM_SCRIPT_EXTENSION_BUFFER_EXPORT static Variable *newVariable(size_t size);
						QUANTUM_SCRIPT_EXTENSION_BUFFER_EXPORT static Variable *newVariable(const uint8_t *data, size_t dataSize);
						QUANTUM_SCRIPT_EXTENSION_BUFFER_EXPORT static Variable *newVariableFromString(String str_);

						QUANTUM_SCRIPT_EXTENSION_BUFFER_EXPORT TPointer<Variable> getPropertyBySymbol(Symbol symbolId);
						QUANTUM_SCRIPT_EXTENSION_BUFFER_EXPORT bool hasPropertyByVariable(Variable *variable);

						QUANTUM_SCRIPT_EXTENSION_BUFFER_EXPORT String getVariableType();

						QUANTUM_SCRIPT_EXTENSION_BUFFER_EXPORT Variable &operatorReference(Symbol symbolId);
						QUANTUM_SCRIPT_EXTENSION_BUFFER_EXPORT Variable *instancePrototype();

						QUANTUM_SCRIPT_EXTENSION_BUFFER_EXPORT static void initMemory();

						QUANTUM_SCRIPT_EXTENSION_BUFFER_EXPORT Variable *clone(SymbolList &inSymbolList);

						QUANTUM_SCRIPT_EXTENSION_BUFFER_EXPORT bool toBoolean();
						QUANTUM_SCRIPT_EXTENSION_BUFFER_EXPORT Number toNumber();
						QUANTUM_SCRIPT_EXTENSION_BUFFER_EXPORT String toString();
						QUANTUM_SCRIPT_EXTENSION_BUFFER_EXPORT bool isString();

						QUANTUM_SCRIPT_EXTENSION_BUFFER_EXPORT void resize(size_t size);
				};

			};
		};
	};
};

#endif
