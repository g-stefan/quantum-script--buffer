//
// Quantum Script Extension Buffer
//
// Copyright (c) 2020-2021 Grigore Stefan <g_stefan@yahoo.com>
// Created by Grigore Stefan <g_stefan@yahoo.com>
//
// MIT License (MIT) <http://opensource.org/licenses/MIT>
//

#ifndef QUANTUM_SCRIPT_EXTENSION_BUFFER_HPP
#define QUANTUM_SCRIPT_EXTENSION_BUFFER_HPP

#ifndef QUANTUM_SCRIPT_HPP
#include "quantum-script.hpp"
#endif

#ifndef QUANTUM_SCRIPT_EXTENSION_BUFFER__EXPORT_HPP
#include "quantum-script-extension-buffer--export.hpp"
#endif

#ifndef QUANTUM_SCRIPT_EXTENSION_BUFFER_COPYRIGHT_HPP
#include "quantum-script-extension-buffer-copyright.hpp"
#endif

#ifndef QUANTUM_SCRIPT_EXTENSION_BUFFER_LICENSE_HPP
#include "quantum-script-extension-buffer-license.hpp"
#endif

#ifndef QUANTUM_SCRIPT_EXTENSION_BUFFER_VERSION_HPP
#include "quantum-script-extension-buffer-version.hpp"
#endif

namespace Quantum {
	namespace Script {
		namespace Extension {
			namespace Buffer {

				using namespace Quantum::Script;

				class BufferContext:
					public Object {
						XYO_DISALLOW_COPY_ASSIGN_MOVE(BufferContext);
					public:

						Symbol symbolFunctionBuffer;
						TPointerX<Prototype> prototypeBuffer;

						QUANTUM_SCRIPT_EXTENSION_BUFFER_EXPORT BufferContext();
				};

				QUANTUM_SCRIPT_EXTENSION_BUFFER_EXPORT BufferContext *getContext();

				QUANTUM_SCRIPT_EXTENSION_BUFFER_EXPORT void initExecutive(Executive *executive, void *extensionId);
				QUANTUM_SCRIPT_EXTENSION_BUFFER_EXPORT void registerInternalExtension(Executive *executive);

			};
		};
	};
};

#endif

