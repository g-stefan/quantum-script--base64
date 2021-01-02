//
// Quantum Script Extension Base64
//
// Copyright (c) 2020-2021 Grigore Stefan <g_stefan@yahoo.com>
// Created by Grigore Stefan <g_stefan@yahoo.com>
//
// MIT License (MIT) <http://opensource.org/licenses/MIT>
//

#ifndef QUANTUM_SCRIPT_EXTENSION_BASE64_COPYRIGHT_HPP
#define QUANTUM_SCRIPT_EXTENSION_BASE64_COPYRIGHT_HPP

#define QUANTUM_SCRIPT_EXTENSION_BASE64_COPYRIGHT            "Copyright (c) Grigore Stefan"
#define QUANTUM_SCRIPT_EXTENSION_BASE64_PUBLISHER            "Grigore Stefan"
#define QUANTUM_SCRIPT_EXTENSION_BASE64_COMPANY              QUANTUM_SCRIPT_EXTENSION_BASE64_PUBLISHER
#define QUANTUM_SCRIPT_EXTENSION_BASE64_CONTACT              "g_stefan@yahoo.com"
#define QUANTUM_SCRIPT_EXTENSION_BASE64_FULL_COPYRIGHT       QUANTUM_SCRIPT_EXTENSION_BASE64_COPYRIGHT " <" QUANTUM_SCRIPT_EXTENSION_BASE64_CONTACT ">"

#ifndef XYO_RC

#ifndef QUANTUM_SCRIPT_EXTENSION_BASE64__EXPORT_HPP
#include "quantum-script-extension-base64--export.hpp"
#endif

namespace Quantum {
	namespace Script {
		namespace Extension {
			namespace Base64 {
				namespace Copyright {
					QUANTUM_SCRIPT_EXTENSION_BASE64_EXPORT const char *copyright();
					QUANTUM_SCRIPT_EXTENSION_BASE64_EXPORT const char *publisher();
					QUANTUM_SCRIPT_EXTENSION_BASE64_EXPORT const char *company();
					QUANTUM_SCRIPT_EXTENSION_BASE64_EXPORT const char *contact();
					QUANTUM_SCRIPT_EXTENSION_BASE64_EXPORT const char *fullCopyright();
				};
			};
		};
	};
};

#endif
#endif
