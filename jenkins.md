## Jenkins Script
#### Template
```
pipeline {
    agent {
        label 'slave'
    }
    parameters {
        text(name: 'text', defaultValue: 'default text', description: 'text')
        choice(name: 'choise', choices: ['one', 'two', 'three'], description: 'choice')
        booleanParam(name: 'bool', defaultValue: true, description: 'bool')
    }
    stages {
        stage('git fetch') {
            steps {
                git url: "http://192.168.176.100:9090/git/hayashi/test.git",  branch: 'master'
            }
        }
        stage('build') {
            steps {
                sh '''
                cd src
                bazel build //...
                '''                
            }
        }
        stage('run') {
            steps {
                sh '''
                cd bazel-bin/src
                ./main
                '''   
            }
        }
        stage('job execution') {
            steps {
                build job: 'build_job'
                build job: 'build_job_with_param', parameters: [
                    [$class: 'StringParameterValue', name: 'text', value: "$text"]
                ]
            }
        }
    }
    post {
        always() {
            sh '''
            bazel clean
            echo $text
            '''
        }
    }
}
```
