PROJECT_ROOT_DIRECTORY = $$PWD

microtexte {
    message(CONFIGURATION MICROTEXTE)

    # QRC ABULEDUSPLASHSCREENV1
    ABULEDUSPLACHSCREENV1_QRC = "$${PWD}/src/microtexte.data/abuledusplashscreenv1/abuledusplashscreenv1.qrc"
    message($$ABULEDUSPLACHSCREENV1_QRC)
    # QRC ABULEDUAPROPOSV1_QRC
    ABULEDUAPROPOSV1_QRC = "$${PWD}/src/microtexte.data/abuleduaproposv1/abuleduaproposv1.qrc"
    message($$ABULEDUAPROPOSV1_QRC)
}
else {
    message(CONFIGURATION MINITEXTE)

    # QRC ABULEDUSPLASHSCREENV1
    ABULEDUSPLACHSCREENV1_QRC = "$${PWD}/src/minitexte.data/abuledusplashscreenv1/abuledusplashscreenv1.qrc"
    message($$ABULEDUSPLACHSCREENV1_QRC)
    # QRC ABULEDUAPROPOSV1_QRC
    ABULEDUAPROPOSV1_QRC = "$${PWD}/src/minitexte.data/abuleduaproposv1/abuleduaproposv1.qrc"
    message($$ABULEDUAPROPOSV1_QRC)
}
