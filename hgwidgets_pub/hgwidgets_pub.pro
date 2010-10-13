# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
# Initial Contributors:
# Nokia Corporation - initial contribution.
# Contributors:
# Description:

TEMPLATE = subdirs

SYMBIAN_PLATFORMS = WINSCW ARMV5

CONFIG += ordered

include(ganeswidgets_api/ganeswidgets_api.pri)
include(hgcacheproxymodel_api/hgcacheproxymodel_api.pri)

symbian {
    headers.sources = $$PUBLIC_HEADERS
    BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>" 
    for(header, headers.sources) {
        FILENAME = $$basename(header)
        BLD_INF_RULES.prj_exports += "$$header MW_LAYER_PUBLIC_EXPORT_PATH(hgwidgets/$$FILENAME)"
        BLD_INF_RULES.prj_exports += "$$header MW_LAYER_PLATFORM_EXPORT_PATH(hgwidgets/$$FILENAME)"
    }
}