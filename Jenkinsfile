pipeline {
    agent { label "linux" }
    options {
        disableConcurrentBuilds()
        skipStagesAfterUnstable()
    }
    stages {
        stage("Tests reports") {
            steps {
                sh "make tests"
                publishHTML([
                    allowMissing: false,
                    alwaysLinkToLastBuild: false,
                    escapeUnderscores: false,
                    includes: "**/*.html, **/*.css",
                    keepAll: true,
                    reportDir: "build/reports",
                    reportFiles: "coverage.html",
                    reportName: "HTML Report",
                ])
                publishCoverage adapters: [
                    cobertura("build/reports/coverage.xml")
                ]
            }
        }
        stage ("Build") {
            when { buildingTag() }
            steps {
                sh "make"
            }
        }
    }
}
