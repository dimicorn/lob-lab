# Contributing to LOB-Lab

Build, test, and lint commands are in [README.md](README.md). Scope and milestones are in [overview.md](overview.md).

## Branches and pull requests

`main` is protected: it accepts changes only through a pull request whose `ci-ok` check passed.
Force-pushes and deleting `main` are blocked, and nobody can bypass this, admins included. PRs are
merged with **squash**, which makes **the PR title the commit message on `main`**. So the PR title
must follow the commit format below.

## Commit messages

Use [Conventional Commits](https://www.conventionalcommits.org/): `type(scope): summary`.

| Type | Use for | CHANGELOG section |
|---|---|---|
| `feat` | New functionality | Features |
| `fix` | Bug fixes | Bug Fixes |
| `perf` | Performance changes (link the benchmark) | Performance |
| `refactor` | Code changes with no behavior change | Refactor |
| `test` | Tests only | Testing |
| `docs` | Documentation only | Documentation |
| `style` | Formatting only | Styling |
| `build`, `ci`, `chore` | Cargo/CMake, CI, maintenance | Miscellaneous Tasks |
| `revert` | Reverting a commit | Revert |

The scope is optional and names the subsystem, e.g. `ffi`, `codec`, `parser`, `book`, `replay`,
`features`, `ml`, `inference`, `bench`, `backtest`. Mark breaking changes (for example, an ABI change
that bumps `LOB_FFI_ABI_VERSION`) with `!`: `feat(ffi)!: switch to caller-allocated batches`.

[git-cliff](https://git-cliff.org/) builds `CHANGELOG.md` from these prefixes using `cliff.toml`.
**Commits without a recognized prefix are left out of the changelog.**

Preview what the next release would contain (without `-o -`, git-cliff overwrites CHANGELOG.md):

```sh
uvx git-cliff --unreleased -o -
```

## Releases

The version is stored in two places that must match: `version` under `[workspace.package]` in
`Cargo.toml`, and `project(lob_lab VERSION …)` in `CMakeLists.txt`. Releases also go through a PR:

```sh
git switch main && git pull
git switch -c release/v0.2.0
# bump the version in Cargo.toml ([workspace.package]) and CMakeLists.txt (project VERSION)
cargo update --workspace                      # refresh Cargo.lock
uvx git-cliff --tag v0.2.0 -o CHANGELOG.md
git commit -am "chore(release): v0.2.0" && git push -u origin release/v0.2.0
gh pr create --fill
# after the PR is merged:
git switch main && git pull
git tag -a v0.2.0 -m v0.2.0 && git push origin v0.2.0
```

Pushing the tag runs `.github/workflows/release.yml`. It checks that the tag matches both version
numbers, then publishes a GitHub Release using that version's section of the changelog.
