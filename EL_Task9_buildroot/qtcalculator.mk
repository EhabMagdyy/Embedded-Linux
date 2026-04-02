################################################################################
#
# itisimplecal
#
################################################################################

QTCALCULATOR_VERSION = 1.0
QTCALCULATOR_SITE = $(QTCALCULATOR_PKGDIR)
QTCALCULATOR_SITE_METHOD = local
QTCALCULATOR_DEPENDENCIES = qt6base qt6declarative
QTCALCULATOR_INSTALL_TARGET = YES

QTCALCULATOR_CONF_OPTS = \
    -DCMAKE_BUILD_TYPE=Release

$(eval $(cmake-package))
