/*
 * Rootable.cpp
 * - Interface for non garbage collected object to track garbage collected object.
 *
 * Copyright (c) 2019~2021 numver8638(신진환, Jinhwan Shin)
 * Released under the MIT License.
 * See the LICENSE file in the project root to get more information.
 */
#include <BuildScript/GC/Rootable.h>

#include <BuildScript/GC/Heap.h>

using namespace BuildScript;

GCRootable::GCRootable() {
    GCHeap::AddRoot(this);
}

GCRootable::~GCRootable() noexcept {
    GCHeap::RemoveRoot(this);
}