#!/bin/bash
# Script to create a new release

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to print colored output
print_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check if version argument is provided
if [ $# -eq 0 ]; then
    print_error "No version specified"
    echo "Usage: $0 <version>"
    echo "Example: $0 1.1.0"
    exit 1
fi

VERSION=$1
TAG="v${VERSION}"

# Validate version format (semantic versioning)
if ! [[ $VERSION =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
    print_error "Invalid version format. Use semantic versioning (e.g., 1.0.0)"
    exit 1
fi

print_info "Creating release ${TAG}"

# Check if we're on main branch
CURRENT_BRANCH=$(git branch --show-current)
if [ "$CURRENT_BRANCH" != "main" ]; then
    print_warning "You are not on the main branch (current: $CURRENT_BRANCH)"
    read -p "Continue anyway? (y/N) " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        exit 1
    fi
fi

# Check for uncommitted changes
if ! git diff-index --quiet HEAD --; then
    print_error "You have uncommitted changes. Please commit or stash them first."
    exit 1
fi

# Pull latest changes
print_info "Pulling latest changes..."
git pull origin main

# Run tests
print_info "Running tests..."
if [ -d "build" ]; then
    cd build
    if ! ctest --output-on-failure; then
        print_error "Tests failed. Fix them before creating a release."
        exit 1
    fi
    cd ..
else
    print_warning "Build directory not found. Skipping tests."
fi

# Update CHANGELOG.md
print_info "Please update CHANGELOG.md with release notes for version ${VERSION}"
read -p "Press enter when done..."

# Check if CHANGELOG was updated
if ! grep -q "\[${VERSION}\]" CHANGELOG.md; then
    print_warning "CHANGELOG.md doesn't contain version ${VERSION}"
    read -p "Continue anyway? (y/N) " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        exit 1
    fi
fi

# Commit CHANGELOG if modified
if ! git diff --quiet CHANGELOG.md; then
    print_info "Committing CHANGELOG.md..."
    git add CHANGELOG.md
    git commit -m "docs: update CHANGELOG for ${TAG}"
fi

# Create and push tag
print_info "Creating tag ${TAG}..."
git tag -a "${TAG}" -m "Release ${TAG}"

print_info "Pushing tag to origin..."
git push origin "${TAG}"

print_info "Pushing commits to origin..."
git push origin main

print_info "✅ Release ${TAG} created successfully!"
echo ""
echo "Next steps:"
echo "1. GitHub Actions will automatically build and create the release"
echo "2. Go to https://github.com/$(git config --get remote.origin.url | sed 's/.*github.com[:/]\(.*\)\.git/\1/')/releases"
echo "3. Edit the release notes if needed"
echo "4. Announce the release"
