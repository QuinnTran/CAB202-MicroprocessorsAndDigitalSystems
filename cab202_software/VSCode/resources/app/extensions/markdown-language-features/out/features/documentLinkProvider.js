"use strict";
/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License. See License.txt in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
Object.defineProperty(exports, "__esModule", { value: true });
const vscode = require("vscode");
const path = require("path");
const openDocumentLink_1 = require("../commands/openDocumentLink");
function normalizeLink(document, link, base) {
    const uri = vscode.Uri.parse(link);
    if (uri.scheme) {
        return uri;
    }
    // assume it must be a file
    let resourcePath = uri.path;
    if (!uri.path) {
        resourcePath = document.uri.path;
    }
    else if (uri.path[0] === '/') {
        const root = vscode.workspace.getWorkspaceFolder(document.uri);
        if (root) {
            resourcePath = path.join(root.uri.fsPath, uri.path);
        }
    }
    else {
        resourcePath = path.join(base, uri.path);
    }
    return openDocumentLink_1.OpenDocumentLinkCommand.createCommandUri(resourcePath, uri.fragment);
}
function matchAll(pattern, text) {
    const out = [];
    pattern.lastIndex = 0;
    let match;
    while ((match = pattern.exec(text))) {
        out.push(match);
    }
    return out;
}
class LinkProvider {
    constructor() {
        this.linkPattern = /(\[[^\]]*\]\(\s*)((([^\s\(\)]|\(\S*?\))+))\s*(".*?")?\)/g;
        this.referenceLinkPattern = /(\[([^\]]+)\]\[\s*?)([^\s\]]*?)\]/g;
        this.definitionPattern = /^([\t ]*\[([^\]]+)\]:\s*)(\S+)/gm;
    }
    provideDocumentLinks(document, _token) {
        const base = path.dirname(document.uri.fsPath);
        const text = document.getText();
        return this.providerInlineLinks(text, document, base)
            .concat(this.provideReferenceLinks(text, document, base));
    }
    providerInlineLinks(text, document, base) {
        const results = [];
        for (const match of matchAll(this.linkPattern, text)) {
            const pre = match[1];
            const link = match[2];
            const offset = (match.index || 0) + pre.length;
            const linkStart = document.positionAt(offset);
            const linkEnd = document.positionAt(offset + link.length);
            try {
                results.push(new vscode.DocumentLink(new vscode.Range(linkStart, linkEnd), normalizeLink(document, link, base)));
            }
            catch (e) {
                // noop
            }
        }
        return results;
    }
    provideReferenceLinks(text, document, base) {
        const results = [];
        const definitions = this.getDefinitions(text, document);
        for (const match of matchAll(this.referenceLinkPattern, text)) {
            let linkStart;
            let linkEnd;
            let reference = match[3];
            if (reference) { // [text][ref]
                const pre = match[1];
                const offset = (match.index || 0) + pre.length;
                linkStart = document.positionAt(offset);
                linkEnd = document.positionAt(offset + reference.length);
            }
            else if (match[2]) { // [ref][]
                reference = match[2];
                const offset = (match.index || 0) + 1;
                linkStart = document.positionAt(offset);
                linkEnd = document.positionAt(offset + match[2].length);
            }
            else {
                continue;
            }
            try {
                const link = definitions.get(reference);
                if (link) {
                    results.push(new vscode.DocumentLink(new vscode.Range(linkStart, linkEnd), vscode.Uri.parse(`command:_markdown.moveCursorToPosition?${encodeURIComponent(JSON.stringify([link.linkRange.start.line, link.linkRange.start.character]))}`)));
                }
            }
            catch (e) {
                // noop
            }
        }
        for (const definition of Array.from(definitions.values())) {
            try {
                results.push(new vscode.DocumentLink(definition.linkRange, normalizeLink(document, definition.link, base)));
            }
            catch (e) {
                // noop
            }
        }
        return results;
    }
    getDefinitions(text, document) {
        const out = new Map();
        for (const match of matchAll(this.definitionPattern, text)) {
            const pre = match[1];
            const reference = match[2];
            const link = match[3].trim();
            const offset = (match.index || 0) + pre.length;
            const linkStart = document.positionAt(offset);
            const linkEnd = document.positionAt(offset + link.length);
            out.set(reference, {
                link: link,
                linkRange: new vscode.Range(linkStart, linkEnd)
            });
        }
        return out;
    }
}
exports.default = LinkProvider;
//# sourceMappingURL=https://ticino.blob.core.windows.net/sourcemaps/1dfc5e557209371715f655691b1235b6b26a06be/extensions\markdown-language-features\out/features\documentLinkProvider.js.map
