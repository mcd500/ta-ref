# gitlab settings

## register runner with `gitlab-runner register`

First, exec `gitlab-runner register` on the gitlab machine to use docker image:

```sh
# This token is checked at Settings > CI/CD > Specific Runners
export TOKEN=AaAaAaAaAaAaAaAaAaAa
# --docker-image[required]: Default docker image
sudo gitlab-runner register --non-interactive --docker-pull-policy "if-not-present" --url "https://192.168.100.100/" --registration-token ${TOKEN} --name ref-ta --tag-list ref-ta --executor docker --docker-image ubuntu:20.04
Runtime platform                                    arch=amd64 os=linux pid=19618 revision=21cb397c version=13.0.1
Running in system-mode.

Registering runner... succeeded                     runner=Rgo2sZ2Z
Runner registered successfully. Feel free to start it, but if it's running already the config should be automatically reloaded!
```

This settings is reflected on `/etc/gitlab-runner/config.toml`

```sh
# see https://qiita.com/South_/items/f9cc9f1413d52c076fd2#%E4%B8%A6%E5%88%97%E5%AE%9F%E8%A1%8C%E6%95%B0%E3%82%92%E5%A2%97%E3%82%84%E3%81%99
concurrent = 6
check_interval = 0
[[runners]]
  name = "ref-ta"
  url = "http://192.168.100.100/"
  token = "9ZLTJXPUHWsW_ZqXLi9S"
  executor = "docker"
  [runners.custom_build_dir]
  [runners.cache]
    [runners.cache.s3]
    [runners.cache.gcs]
  [runners.docker]
    tls_verify = false
    image = "ubuntu:20.04"
    privileged = false
    disable_entrypoint_overwrite = false
    oom_kill_disable = false
    disable_cache = false
    volumes = ["/cache"]
    # Necessary If the private image on local machine is used.
    pull_policy = "if-not-present"
    shm_size = 0
```

<<<<<<< HEAD
## gitlab-ci.yml settings

For each repository, configure CI in yml file, gitlab-ci.yml. In gitlab-ci.yml, specify `tags:` with `ref-ta` as follows:
=======
## gitlab-ci settings

In gitlab-ci, specify `tags` with `ref-ta` as follows:
>>>>>>> 24fcd72... Fix ci

```yml
job1:
    image: ${PRIVATE_IMAGE}
    variables:
        STH: mm
    only:
        - master
    tags:
        - ref-ta
    stage: test
    script:
        - echo "success!"
```
