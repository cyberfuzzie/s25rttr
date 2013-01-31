##S25RTTR

This is a fork of **Settlers II - Return to the Roots** (aka Settlers 2.5).
S25RTTR is a re-programming of the popular Settlers II, using only the graphics sprites and data blobs from the original, but implementing the game logic newly, allowing you to make use of newer features like higher resolutions and better graphics drivers.

The official website can be found at http://www.siedler25.org

This fork will be used to play around with the source and try to make improvements. Some or all of our changes may be included into the official s25rttr ("upstream") repository, but that is not our main goal. We're mostly trying to improve the game we love to play and expand our programming knowledge at the same time.

S25RTTR is released under the GNU GPLv3 (see [LICENSE](LICENSE) for full text).

##A note on this repo's displayed history

We imported the s25rttr repo from [their Launchpad bzr repo repo at r8581](http://bazaar.launchpad.net/~flosoft/s25rttr/trunk/revision/8581), but they're using a weird commit strategy. Apparently the s25rttr core team uses an internal SVN repo for their actual code tracking, the commits to which are automatically copied to the lp bzr. As they didn't do that from the very beginning, but presumably wanted to have the same revision numbers, they started out with approx. 3000 empty commits (containing only the revision number). (Funnily enough, the rev numbers have de-synced by now.)

Secondly, something in their setup makes a lot of empty commits (those are literally empty, as in the diff is empty; there's merely an automated commit message), possibly an automated build setup or so.

So after converting the repo (bzr fast-export -- git fast-import), we removed those pointless commits in the beginning, and had git remove every empty commit, reducing the history of the repo from ~8500 to ~2400 commits and the repo size from ~180MB to ~130MB.

The history graphs on this repo are thus quite accurate (where number of commits are involved), assuming the authors were added correctly to the bzr repo (converting them to git worked correctly).

##A note on how we keep up-to-date with upstream

Since the core dev team of s25rttr is also somewhat active, there are bound to be upstream changes while we work on this fork. We'll add a commit for every (non-empty) bzr revision to the branch "bazaar-integration", and add a tag with the corresponding bzr rev number whenever we merge that branch into master.

Please remember that we only just created this fork and are not 100% sure on this strategy yet. We're going to try to keep up the author(s) for the bzr integration commits, so the core devs are appropriately credited.

