//
// Quantum Script Extension Base64
//
// Copyright (c) 2020-2022 Grigore Stefan <g_stefan@yahoo.com>
// Created by Grigore Stefan <g_stefan@yahoo.com>
//
// MIT License (MIT) <http://opensource.org/licenses/MIT>
//

#ifndef QUANTUM_SCRIPT_EXTENSION_BASE64_HPP
#define QUANTUM_SCRIPT_EXTENSION_BASE64_HPP

#ifndef QUANTUM_SCRIPT_HPP
#include "quantum-script.hpp"
#endif

#ifndef QUANTUM_SCRIPT_EXTENSION_BASE64__EXPORT_HPP
#include "quantum-script-extension-base64--export.hpp"
#endif

#ifndef QUANTUM_SCRIPT_EXTENSION_BASE64_COPYRIGHT_HPP
#include "quantum-script-extension-base64-copyright.hpp"
#endif

#ifndef QUANTUM_SCRIPT_EXTENSION_BASE64_LICENSE_HPP
#include "quantum-script-extension-base64-license.hpp"
#endif

#ifndef QUANTUM_SCRIPT_EXTENSION_BASE64_VERSION_HPP
#include "quantum-script-extension-base64-version.hpp"
#endif

namespace Quantum {
	namespace Script {
		namespace Extension {
			namespace Base64 {

				using namespace Quantum::Script;

				QUANTUM_SCRIPT_EXTENSION_BASE64_EXPORT void initExecutive(Executive *executive, void *extensionId);
				QUANTUM_SCRIPT_EXTENSION_BASE64_EXPORT void registerInternalExtension(Executive *executive);

			};
		};
	};
};

#endif

