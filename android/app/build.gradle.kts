plugins {
    id("com.android.application")
}

android {
    namespace = "com.itgmania.game"
    compileSdk = 34
    ndkVersion = "25.2.9519653"

    defaultConfig {
        applicationId = "com.itgmania.game"
        minSdk = 26
        targetSdk = 34
        versionCode = 1
        versionName = "0.8.0-android"

        ndk {
            abiFilters += listOf("arm64-v8a", "armeabi-v7a")
        }

        externalNativeBuild {
            cmake {
                arguments += listOf(
                    "-DANDROID_STL=c++_shared",
                    "-DCMAKE_BUILD_TYPE=Release"
                )
            }
        }
    }

    buildTypes {
        release {
            isMinifyEnabled = false
            proguardFiles(
                getDefaultProguardFile("proguard-android-optimize.txt"),
                "proguard-rules.pro"
            )
        }
        debug {
            isDebuggable = true
            isJniDebuggable = true
        }
    }

    externalNativeBuild {
        cmake {
            path = file("src/main/cpp/CMakeLists.txt")
            version = "3.22.1"
        }
    }

    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_17
        targetCompatibility = JavaVersion.VERSION_17
    }

    // Include SDL2 Java sources
    sourceSets {
        getByName("main") {
            java.srcDirs(
                "src/main/java",
                "../../extern/SDL/android-project/app/src/main/java"
            )
        }
    }

}

dependencies {
    // No external dependencies needed - using native SDL2
}
