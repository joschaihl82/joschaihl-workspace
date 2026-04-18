const __vite__mapDeps=(i,m=__vite__mapDeps,d=(m.f||(m.f=["offshoot_assets/donate-page-DRqrw3i6.js","offshoot_assets/index-SNvmkimM.js","offshoot_assets/donation-form-section-D-e9cfzm.js","offshoot_assets/index-BxCKNB1V.js","offshoot_assets/index-Bj--Zo_c.js","offshoot_assets/icon-close-B0o8ppg8.js","offshoot_assets/unsafe-html-C0bAGlMm.js","offshoot_assets/details-page-router-BNrFkR3B.js","offshoot_assets/choose-lny907YL.js","offshoot_assets/account-settings-pYmZ8_sV.js","offshoot_assets/log-DC4A_mvZ.js","offshoot_assets/third-party-auth-service-DIrx_kSb.js","offshoot_assets/index-D3NNTP1P.js","offshoot_assets/ia-status-indicator-CGVsxapT.js","offshoot_assets/ia-text-divider-Bi4ZfBTn.js","offshoot_assets/ia-logo-gray-circle-Dzr9jDvQ.js","offshoot_assets/ia-button-DjTTINvO.js","offshoot_assets/when-C3ujTgb2.js","offshoot_assets/email-link-D_r2hdVW.js","offshoot_assets/ia-icon-0xg6gnMy.js","offshoot_assets/ia-pic-uploader-BOeJfOaH.js","offshoot_assets/model-DdwzJq09.js","offshoot_assets/add-light-BRYetma-.js","offshoot_assets/exports-8TZx4202.js","offshoot_assets/close-BufSyB0i.js","offshoot_assets/purify.es-CGs5VtEq.js","offshoot_assets/search-page-D1iE3zZz.js","offshoot_assets/directive-helpers-5QFAHbzi.js","offshoot_assets/capitalize-97Y2ZK2e.js","offshoot_assets/style-map-BhW7ENxW.js","offshoot_assets/search-params-DMx1IvAT.js","offshoot_assets/review-DeK3KHAS.js","offshoot_assets/string-Bnnka0r4.js","offshoot_assets/action-bar-DYsC9xai.js","offshoot_assets/ia-clearable-text-input-Cm69zvz7.js","offshoot_assets/index-BCMOcpuB.js","offshoot_assets/share-modal-content-B-D-Emcn.js","offshoot_assets/ia-survey-extra-DPIPKDq8.js","offshoot_assets/rss-B8jbkGBz.js","offshoot_assets/edit-CiJFIHNZ.js","offshoot_assets/tile-carousel-CP0evC_Z.js","offshoot_assets/tab-manager-DfU8jNs6.js","offshoot_assets/collection-search-input-CTBy6TLV.js","offshoot_assets/external-link-D0FXuV-9.js","offshoot_assets/wikipedia-api-service-DkgQwSo_.js","offshoot_assets/friendly-truncate--jIWhF6M.js","offshoot_assets/toggle-switch-Uh9ClLt_.js","offshoot_assets/home-page-D50WaHLp.js","offshoot_assets/schema-org-json-ld-service-BpkiL_eM.js","offshoot_assets/web-QQCcHJiw.js","offshoot_assets/collection-D0LxFch-.js","offshoot_assets/basic-carousel-BvRgjEnr.js","offshoot_assets/login-page-BDbdnlJv.js","offshoot_assets/forgot-password-PqH6jkkY.js","offshoot_assets/subscribe-form-CZH5M5Cd.js","offshoot_assets/static-page-BN1ilDG4.js","offshoot_assets/ia-forum-C1mbwmx4.js","offshoot_assets/discover-page-DlB9tJgE.js","offshoot_assets/details-page-BGRQ2-s1.js","offshoot_assets/index-BzK71pux.js","offshoot_assets/normalize-newlines-BieJKn4Z.js","offshoot_assets/index-i1WwzfoA.js","offshoot_assets/truncate-screenname-Boj5Oc7J.js","offshoot_assets/metadata-search-link-B2a3qsdj.js","offshoot_assets/sentry-AjyKTUQ5.js","offshoot_assets/psa-banners-CJ8F4s-y.js","offshoot_assets/alert-banner-D5y_Mm5l.js","offshoot_assets/index-jBrPYLBf.js","offshoot_assets/dev-tools-BHvjMMdg.js","offshoot_assets/localization-manager-p-vaYL8Z.js","offshoot_assets/theme-option-BPtoJbAF.js"])))=>i.map(i=>d[i]);
(function(){const e=document.createElement("link").relList;if(e&&e.supports&&e.supports("modulepreload"))return;for(const n of document.querySelectorAll('link[rel="modulepreload"]'))r(n);new MutationObserver(n=>{for(const s of n)if(s.type==="childList")for(const o of s.addedNodes)o.tagName==="LINK"&&o.rel==="modulepreload"&&r(o)}).observe(document,{childList:!0,subtree:!0});function t(n){const s={};return n.integrity&&(s.integrity=n.integrity),n.referrerPolicy&&(s.referrerPolicy=n.referrerPolicy),n.crossOrigin==="use-credentials"?s.credentials="include":n.crossOrigin==="anonymous"?s.credentials="omit":s.credentials="same-origin",s}function r(n){if(n.ep)return;n.ep=!0;const s=t(n);fetch(n.href,s)}})();const Ji="modulepreload",er=function(i){return"/"+i},Ot={},f=function(e,t,r){let n=Promise.resolve();if(t&&t.length>0){let o=function(d){return Promise.all(d.map(u=>Promise.resolve(u).then(m=>({status:"fulfilled",value:m}),m=>({status:"rejected",reason:m}))))};document.getElementsByTagName("link");const l=document.querySelector("meta[property=csp-nonce]"),a=(l==null?void 0:l.nonce)||(l==null?void 0:l.getAttribute("nonce"));n=o(t.map(d=>{if(d=er(d),d in Ot)return;Ot[d]=!0;const u=d.endsWith(".css"),m=u?'[rel="stylesheet"]':"";if(document.querySelector(`link[href="${d}"]${m}`))return;const v=document.createElement("link");if(v.rel=u?"stylesheet":Ji,u||(v.as="script"),v.crossOrigin="",v.href=d,a&&v.setAttribute("nonce",a),document.head.appendChild(v),u)return new Promise((b,x)=>{v.addEventListener("load",b),v.addEventListener("error",()=>x(new Error(`Unable to preload CSS for ${d}`)))})}))}function s(o){const l=new Event("vite:preloadError",{cancelable:!0});if(l.payload=o,window.dispatchEvent(l),!l.defaultPrevented)throw o}return n.then(o=>{for(const l of o||[])l.status==="rejected"&&s(l.reason);return e().catch(s)})};/**
 * @license
 * Copyright 2021 Google LLC
 * SPDX-License-Identifier: BSD-3-Clause
 */const tr=(i,...e)=>({strTag:!0,strings:i,values:e}),H=tr,ir=i=>typeof i!="string"&&"strTag"in i,vi=(i,e,t)=>{let r=i[0];for(let n=1;n<i.length;n++)r+=e[t?t[n-1]:n-1],r+=i[n];return r};/**
 * @license
 * Copyright 2021 Google LLC
 * SPDX-License-Identifier: BSD-3-Clause
 */const gi=(i=>ir(i)?vi(i.strings,i.values):i);/**
 * @license
 * Copyright 2021 Google LLC
 * SPDX-License-Identifier: BSD-3-Clause
 */const rr="lit-localize-status";/**
 * @license
 * Copyright 2020 Google LLC
 * SPDX-License-Identifier: BSD-3-Clause
 */class fi{constructor(){this.settled=!1,this.promise=new Promise((e,t)=>{this._resolve=e,this._reject=t})}resolve(e){this.settled=!0,this._resolve(e)}reject(e){this.settled=!0,this._reject(e)}}/**
 * @license
 * Copyright 2014 Travis Webb
 * SPDX-License-Identifier: MIT
 */const N=[];for(let i=0;i<256;i++)N[i]=(i>>4&15).toString(16)+(i&15).toString(16);function nr(i){let e=0,t=8997,r=0,n=33826,s=0,o=40164,l=0,a=52210;for(let d=0;d<i.length;d++)t^=i.charCodeAt(d),e=t*435,r=n*435,s=o*435,l=a*435,s+=t<<8,l+=n<<8,r+=e>>>16,t=e&65535,s+=r>>>16,n=r&65535,a=l+(s>>>16)&65535,o=s&65535;return N[a>>8]+N[a&255]+N[o>>8]+N[o&255]+N[n>>8]+N[n&255]+N[t>>8]+N[t&255]}/**
 * @license
 * Copyright 2020 Google LLC
 * SPDX-License-Identifier: BSD-3-Clause
 */const sr="",or="h",ar="s";function lr(i,e){return(e?or:ar)+nr(typeof i=="string"?i:i.join(sr))}/**
 * @license
 * Copyright 2021 Google LLC
 * SPDX-License-Identifier: BSD-3-Clause
 */const Dt=new WeakMap,Ut=new Map;function cr(i,e,t){var r;if(i){const n=(r=t==null?void 0:t.id)!==null&&r!==void 0?r:dr(e),s=i[n];if(s){if(typeof s=="string")return s;if("strTag"in s)return vi(s.strings,e.values,s.values);{let o=Dt.get(s);return o===void 0&&(o=s.values,Dt.set(s,o)),{...s,values:o.map(l=>e.values[l])}}}}return gi(e)}function dr(i){const e=typeof i=="string"?i:i.strings;let t=Ut.get(e);return t===void 0&&(t=lr(e,typeof i!="string"&&!("strTag"in i)),Ut.set(e,t)),t}/**
 * @license
 * Copyright 2021 Google LLC
 * SPDX-License-Identifier: BSD-3-Clause
 */function Ye(i){window.dispatchEvent(new CustomEvent(rr,{detail:i}))}let Pe="",ve,yi,Me,ut,bi,G=new fi;G.resolve();let Ce=0;const Os=i=>(pr(((e,t)=>cr(bi,e,t))),Pe=yi=i.sourceLocale,Me=new Set(i.targetLocales),Me.add(i.sourceLocale),ut=i.loadLocale,{getLocale:hr,setLocale:ur}),hr=()=>Pe,ur=i=>{if(i===(ve??Pe))return G.promise;if(!Me||!ut)throw new Error("Internal error");if(!Me.has(i))throw new Error("Invalid locale code");Ce++;const e=Ce;return ve=i,G.settled&&(G=new fi),Ye({status:"loading",loadingLocale:i}),(i===yi?Promise.resolve({templates:void 0}):ut(i)).then(r=>{Ce===e&&(Pe=i,ve=void 0,bi=r.templates,Ye({status:"ready",readyLocale:i}),G.resolve())},r=>{Ce===e&&(Ye({status:"error",errorLocale:i,errorMessage:r.toString()}),G.reject(r))}),G.promise};/**
 * @license
 * Copyright 2020 Google LLC
 * SPDX-License-Identifier: BSD-3-Clause
 */let M=gi,Nt=!1;function pr(i){if(Nt)throw new Error("lit-localize can only be configured once");M=i,Nt=!0}/**
 * @license
 * Copyright 2019 Google LLC
 * SPDX-License-Identifier: BSD-3-Clause
 */const xe=window,St=xe.ShadowRoot&&(xe.ShadyCSS===void 0||xe.ShadyCSS.nativeShadow)&&"adoptedStyleSheets"in Document.prototype&&"replace"in CSSStyleSheet.prototype,Et=Symbol(),zt=new WeakMap;let wi=class{constructor(e,t,r){if(this._$cssResult$=!0,r!==Et)throw Error("CSSResult is not constructable. Use `unsafeCSS` or `css` instead.");this.cssText=e,this.t=t}get styleSheet(){let e=this.o;const t=this.t;if(St&&e===void 0){const r=t!==void 0&&t.length===1;r&&(e=zt.get(t)),e===void 0&&((this.o=e=new CSSStyleSheet).replaceSync(this.cssText),r&&zt.set(t,e))}return e}toString(){return this.cssText}};const mr=i=>new wi(typeof i=="string"?i:i+"",void 0,Et),g=(i,...e)=>{const t=i.length===1?i[0]:e.reduce(((r,n,s)=>r+(o=>{if(o._$cssResult$===!0)return o.cssText;if(typeof o=="number")return o;throw Error("Value passed to 'css' function must be a 'css' function result: "+o+". Use 'unsafeCSS' to pass non-literal values, but take care to ensure page security.")})(n)+i[s+1]),i[0]);return new wi(t,i,Et)},vr=(i,e)=>{St?i.adoptedStyleSheets=e.map((t=>t instanceof CSSStyleSheet?t:t.styleSheet)):e.forEach((t=>{const r=document.createElement("style"),n=xe.litNonce;n!==void 0&&r.setAttribute("nonce",n),r.textContent=t.cssText,i.appendChild(r)}))},Vt=St?i=>i:i=>i instanceof CSSStyleSheet?(e=>{let t="";for(const r of e.cssRules)t+=r.cssText;return mr(t)})(i):i;/**
 * @license
 * Copyright 2017 Google LLC
 * SPDX-License-Identifier: BSD-3-Clause
 */var Ze;const Re=window,Ht=Re.trustedTypes,gr=Ht?Ht.emptyScript:"",Ft=Re.reactiveElementPolyfillSupport,pt={toAttribute(i,e){switch(e){case Boolean:i=i?gr:null;break;case Object:case Array:i=i==null?i:JSON.stringify(i)}return i},fromAttribute(i,e){let t=i;switch(e){case Boolean:t=i!==null;break;case Number:t=i===null?null:Number(i);break;case Object:case Array:try{t=JSON.parse(i)}catch{t=null}}return t}},$i=(i,e)=>e!==i&&(e==e||i==i),Qe={attribute:!0,type:String,converter:pt,reflect:!1,hasChanged:$i},mt="finalized";let se=class extends HTMLElement{constructor(){super(),this._$Ei=new Map,this.isUpdatePending=!1,this.hasUpdated=!1,this._$El=null,this._$Eu()}static addInitializer(e){var t;this.finalize(),((t=this.h)!==null&&t!==void 0?t:this.h=[]).push(e)}static get observedAttributes(){this.finalize();const e=[];return this.elementProperties.forEach(((t,r)=>{const n=this._$Ep(r,t);n!==void 0&&(this._$Ev.set(n,r),e.push(n))})),e}static createProperty(e,t=Qe){if(t.state&&(t.attribute=!1),this.finalize(),this.elementProperties.set(e,t),!t.noAccessor&&!this.prototype.hasOwnProperty(e)){const r=typeof e=="symbol"?Symbol():"__"+e,n=this.getPropertyDescriptor(e,r,t);n!==void 0&&Object.defineProperty(this.prototype,e,n)}}static getPropertyDescriptor(e,t,r){return{get(){return this[t]},set(n){const s=this[e];this[t]=n,this.requestUpdate(e,s,r)},configurable:!0,enumerable:!0}}static getPropertyOptions(e){return this.elementProperties.get(e)||Qe}static finalize(){if(this.hasOwnProperty(mt))return!1;this[mt]=!0;const e=Object.getPrototypeOf(this);if(e.finalize(),e.h!==void 0&&(this.h=[...e.h]),this.elementProperties=new Map(e.elementProperties),this._$Ev=new Map,this.hasOwnProperty("properties")){const t=this.properties,r=[...Object.getOwnPropertyNames(t),...Object.getOwnPropertySymbols(t)];for(const n of r)this.createProperty(n,t[n])}return this.elementStyles=this.finalizeStyles(this.styles),!0}static finalizeStyles(e){const t=[];if(Array.isArray(e)){const r=new Set(e.flat(1/0).reverse());for(const n of r)t.unshift(Vt(n))}else e!==void 0&&t.push(Vt(e));return t}static _$Ep(e,t){const r=t.attribute;return r===!1?void 0:typeof r=="string"?r:typeof e=="string"?e.toLowerCase():void 0}_$Eu(){var e;this._$E_=new Promise((t=>this.enableUpdating=t)),this._$AL=new Map,this._$Eg(),this.requestUpdate(),(e=this.constructor.h)===null||e===void 0||e.forEach((t=>t(this)))}addController(e){var t,r;((t=this._$ES)!==null&&t!==void 0?t:this._$ES=[]).push(e),this.renderRoot!==void 0&&this.isConnected&&((r=e.hostConnected)===null||r===void 0||r.call(e))}removeController(e){var t;(t=this._$ES)===null||t===void 0||t.splice(this._$ES.indexOf(e)>>>0,1)}_$Eg(){this.constructor.elementProperties.forEach(((e,t)=>{this.hasOwnProperty(t)&&(this._$Ei.set(t,this[t]),delete this[t])}))}createRenderRoot(){var e;const t=(e=this.shadowRoot)!==null&&e!==void 0?e:this.attachShadow(this.constructor.shadowRootOptions);return vr(t,this.constructor.elementStyles),t}connectedCallback(){var e;this.renderRoot===void 0&&(this.renderRoot=this.createRenderRoot()),this.enableUpdating(!0),(e=this._$ES)===null||e===void 0||e.forEach((t=>{var r;return(r=t.hostConnected)===null||r===void 0?void 0:r.call(t)}))}enableUpdating(e){}disconnectedCallback(){var e;(e=this._$ES)===null||e===void 0||e.forEach((t=>{var r;return(r=t.hostDisconnected)===null||r===void 0?void 0:r.call(t)}))}attributeChangedCallback(e,t,r){this._$AK(e,r)}_$EO(e,t,r=Qe){var n;const s=this.constructor._$Ep(e,r);if(s!==void 0&&r.reflect===!0){const o=(((n=r.converter)===null||n===void 0?void 0:n.toAttribute)!==void 0?r.converter:pt).toAttribute(t,r.type);this._$El=e,o==null?this.removeAttribute(s):this.setAttribute(s,o),this._$El=null}}_$AK(e,t){var r;const n=this.constructor,s=n._$Ev.get(e);if(s!==void 0&&this._$El!==s){const o=n.getPropertyOptions(s),l=typeof o.converter=="function"?{fromAttribute:o.converter}:((r=o.converter)===null||r===void 0?void 0:r.fromAttribute)!==void 0?o.converter:pt;this._$El=s,this[s]=l.fromAttribute(t,o.type),this._$El=null}}requestUpdate(e,t,r){let n=!0;e!==void 0&&(((r=r||this.constructor.getPropertyOptions(e)).hasChanged||$i)(this[e],t)?(this._$AL.has(e)||this._$AL.set(e,t),r.reflect===!0&&this._$El!==e&&(this._$EC===void 0&&(this._$EC=new Map),this._$EC.set(e,r))):n=!1),!this.isUpdatePending&&n&&(this._$E_=this._$Ej())}async _$Ej(){this.isUpdatePending=!0;try{await this._$E_}catch(t){Promise.reject(t)}const e=this.scheduleUpdate();return e!=null&&await e,!this.isUpdatePending}scheduleUpdate(){return this.performUpdate()}performUpdate(){var e;if(!this.isUpdatePending)return;this.hasUpdated,this._$Ei&&(this._$Ei.forEach(((n,s)=>this[s]=n)),this._$Ei=void 0);let t=!1;const r=this._$AL;try{t=this.shouldUpdate(r),t?(this.willUpdate(r),(e=this._$ES)===null||e===void 0||e.forEach((n=>{var s;return(s=n.hostUpdate)===null||s===void 0?void 0:s.call(n)})),this.update(r)):this._$Ek()}catch(n){throw t=!1,this._$Ek(),n}t&&this._$AE(r)}willUpdate(e){}_$AE(e){var t;(t=this._$ES)===null||t===void 0||t.forEach((r=>{var n;return(n=r.hostUpdated)===null||n===void 0?void 0:n.call(r)})),this.hasUpdated||(this.hasUpdated=!0,this.firstUpdated(e)),this.updated(e)}_$Ek(){this._$AL=new Map,this.isUpdatePending=!1}get updateComplete(){return this.getUpdateComplete()}getUpdateComplete(){return this._$E_}shouldUpdate(e){return!0}update(e){this._$EC!==void 0&&(this._$EC.forEach(((t,r)=>this._$EO(r,this[r],t))),this._$EC=void 0),this._$Ek()}updated(e){}firstUpdated(e){}};se[mt]=!0,se.elementProperties=new Map,se.elementStyles=[],se.shadowRootOptions={mode:"open"},Ft==null||Ft({ReactiveElement:se}),((Ze=Re.reactiveElementVersions)!==null&&Ze!==void 0?Ze:Re.reactiveElementVersions=[]).push("1.6.3");/**
 * @license
 * Copyright 2017 Google LLC
 * SPDX-License-Identifier: BSD-3-Clause
 */var Xe;const Be=window,oe=Be.trustedTypes,Wt=oe?oe.createPolicy("lit-html",{createHTML:i=>i}):void 0,vt="$lit$",F=`lit$${(Math.random()+"").slice(9)}$`,_i="?"+F,fr=`<${_i}>`,X=document,Oe=()=>X.createComment(""),be=i=>i===null||typeof i!="object"&&typeof i!="function",Si=Array.isArray,yr=i=>Si(i)||typeof(i==null?void 0:i[Symbol.iterator])=="function",Je=`[ 	
\f\r]`,ge=/<(?:(!--|\/[^a-zA-Z])|(\/?[a-zA-Z][^>\s]*)|(\/?$))/g,jt=/-->/g,qt=/>/g,q=RegExp(`>|${Je}(?:([^\\s"'>=/]+)(${Je}*=${Je}*(?:[^ 	
\f\r"'\`<>=]|("|')|))|$)`,"g"),Kt=/'/g,Gt=/"/g,Ei=/^(?:script|style|textarea|title)$/i,we=Symbol.for("lit-noChange"),k=Symbol.for("lit-nothing"),Yt=new WeakMap,Y=X.createTreeWalker(X,129,null,!1);function Ai(i,e){if(!Array.isArray(i)||!i.hasOwnProperty("raw"))throw Error("invalid template strings array");return Wt!==void 0?Wt.createHTML(e):e}const br=(i,e)=>{const t=i.length-1,r=[];let n,s=e===2?"<svg>":"",o=ge;for(let l=0;l<t;l++){const a=i[l];let d,u,m=-1,v=0;for(;v<a.length&&(o.lastIndex=v,u=o.exec(a),u!==null);)v=o.lastIndex,o===ge?u[1]==="!--"?o=jt:u[1]!==void 0?o=qt:u[2]!==void 0?(Ei.test(u[2])&&(n=RegExp("</"+u[2],"g")),o=q):u[3]!==void 0&&(o=q):o===q?u[0]===">"?(o=n??ge,m=-1):u[1]===void 0?m=-2:(m=o.lastIndex-u[2].length,d=u[1],o=u[3]===void 0?q:u[3]==='"'?Gt:Kt):o===Gt||o===Kt?o=q:o===jt||o===qt?o=ge:(o=q,n=void 0);const b=o===q&&i[l+1].startsWith("/>")?" ":"";s+=o===ge?a+fr:m>=0?(r.push(d),a.slice(0,m)+vt+a.slice(m)+F+b):a+F+(m===-2?(r.push(void 0),l):b)}return[Ai(i,s+(i[t]||"<?>")+(e===2?"</svg>":"")),r]};let gt=class ki{constructor({strings:e,_$litType$:t},r){let n;this.parts=[];let s=0,o=0;const l=e.length-1,a=this.parts,[d,u]=br(e,t);if(this.el=ki.createElement(d,r),Y.currentNode=this.el.content,t===2){const m=this.el.content,v=m.firstChild;v.remove(),m.append(...v.childNodes)}for(;(n=Y.nextNode())!==null&&a.length<l;){if(n.nodeType===1){if(n.hasAttributes()){const m=[];for(const v of n.getAttributeNames())if(v.endsWith(vt)||v.startsWith(F)){const b=u[o++];if(m.push(v),b!==void 0){const x=n.getAttribute(b.toLowerCase()+vt).split(F),C=/([.?@])?(.*)/.exec(b);a.push({type:1,index:s,name:C[2],strings:x,ctor:C[1]==="."?$r:C[1]==="?"?Sr:C[1]==="@"?Er:Ve})}else a.push({type:6,index:s})}for(const v of m)n.removeAttribute(v)}if(Ei.test(n.tagName)){const m=n.textContent.split(F),v=m.length-1;if(v>0){n.textContent=oe?oe.emptyScript:"";for(let b=0;b<v;b++)n.append(m[b],Oe()),Y.nextNode(),a.push({type:2,index:++s});n.append(m[v],Oe())}}}else if(n.nodeType===8)if(n.data===_i)a.push({type:2,index:s});else{let m=-1;for(;(m=n.data.indexOf(F,m+1))!==-1;)a.push({type:7,index:s}),m+=F.length-1}s++}}static createElement(e,t){const r=X.createElement("template");return r.innerHTML=e,r}};function ae(i,e,t=i,r){var n,s,o,l;if(e===we)return e;let a=r!==void 0?(n=t._$Co)===null||n===void 0?void 0:n[r]:t._$Cl;const d=be(e)?void 0:e._$litDirective$;return(a==null?void 0:a.constructor)!==d&&((s=a==null?void 0:a._$AO)===null||s===void 0||s.call(a,!1),d===void 0?a=void 0:(a=new d(i),a._$AT(i,t,r)),r!==void 0?((o=(l=t)._$Co)!==null&&o!==void 0?o:l._$Co=[])[r]=a:t._$Cl=a),a!==void 0&&(e=ae(i,a._$AS(i,e.values),a,r)),e}let wr=class{constructor(e,t){this._$AV=[],this._$AN=void 0,this._$AD=e,this._$AM=t}get parentNode(){return this._$AM.parentNode}get _$AU(){return this._$AM._$AU}u(e){var t;const{el:{content:r},parts:n}=this._$AD,s=((t=e==null?void 0:e.creationScope)!==null&&t!==void 0?t:X).importNode(r,!0);Y.currentNode=s;let o=Y.nextNode(),l=0,a=0,d=n[0];for(;d!==void 0;){if(l===d.index){let u;d.type===2?u=new At(o,o.nextSibling,this,e):d.type===1?u=new d.ctor(o,d.name,d.strings,this,e):d.type===6&&(u=new Ar(o,this,e)),this._$AV.push(u),d=n[++a]}l!==(d==null?void 0:d.index)&&(o=Y.nextNode(),l++)}return Y.currentNode=X,s}v(e){let t=0;for(const r of this._$AV)r!==void 0&&(r.strings!==void 0?(r._$AI(e,r,t),t+=r.strings.length-2):r._$AI(e[t])),t++}},At=class Ii{constructor(e,t,r,n){var s;this.type=2,this._$AH=k,this._$AN=void 0,this._$AA=e,this._$AB=t,this._$AM=r,this.options=n,this._$Cp=(s=n==null?void 0:n.isConnected)===null||s===void 0||s}get _$AU(){var e,t;return(t=(e=this._$AM)===null||e===void 0?void 0:e._$AU)!==null&&t!==void 0?t:this._$Cp}get parentNode(){let e=this._$AA.parentNode;const t=this._$AM;return t!==void 0&&(e==null?void 0:e.nodeType)===11&&(e=t.parentNode),e}get startNode(){return this._$AA}get endNode(){return this._$AB}_$AI(e,t=this){e=ae(this,e,t),be(e)?e===k||e==null||e===""?(this._$AH!==k&&this._$AR(),this._$AH=k):e!==this._$AH&&e!==we&&this._(e):e._$litType$!==void 0?this.g(e):e.nodeType!==void 0?this.$(e):yr(e)?this.T(e):this._(e)}k(e){return this._$AA.parentNode.insertBefore(e,this._$AB)}$(e){this._$AH!==e&&(this._$AR(),this._$AH=this.k(e))}_(e){this._$AH!==k&&be(this._$AH)?this._$AA.nextSibling.data=e:this.$(X.createTextNode(e)),this._$AH=e}g(e){var t;const{values:r,_$litType$:n}=e,s=typeof n=="number"?this._$AC(e):(n.el===void 0&&(n.el=gt.createElement(Ai(n.h,n.h[0]),this.options)),n);if(((t=this._$AH)===null||t===void 0?void 0:t._$AD)===s)this._$AH.v(r);else{const o=new wr(s,this),l=o.u(this.options);o.v(r),this.$(l),this._$AH=o}}_$AC(e){let t=Yt.get(e.strings);return t===void 0&&Yt.set(e.strings,t=new gt(e)),t}T(e){Si(this._$AH)||(this._$AH=[],this._$AR());const t=this._$AH;let r,n=0;for(const s of e)n===t.length?t.push(r=new Ii(this.k(Oe()),this.k(Oe()),this,this.options)):r=t[n],r._$AI(s),n++;n<t.length&&(this._$AR(r&&r._$AB.nextSibling,n),t.length=n)}_$AR(e=this._$AA.nextSibling,t){var r;for((r=this._$AP)===null||r===void 0||r.call(this,!1,!0,t);e&&e!==this._$AB;){const n=e.nextSibling;e.remove(),e=n}}setConnected(e){var t;this._$AM===void 0&&(this._$Cp=e,(t=this._$AP)===null||t===void 0||t.call(this,e))}},Ve=class{constructor(e,t,r,n,s){this.type=1,this._$AH=k,this._$AN=void 0,this.element=e,this.name=t,this._$AM=n,this.options=s,r.length>2||r[0]!==""||r[1]!==""?(this._$AH=Array(r.length-1).fill(new String),this.strings=r):this._$AH=k}get tagName(){return this.element.tagName}get _$AU(){return this._$AM._$AU}_$AI(e,t=this,r,n){const s=this.strings;let o=!1;if(s===void 0)e=ae(this,e,t,0),o=!be(e)||e!==this._$AH&&e!==we,o&&(this._$AH=e);else{const l=e;let a,d;for(e=s[0],a=0;a<s.length-1;a++)d=ae(this,l[r+a],t,a),d===we&&(d=this._$AH[a]),o||(o=!be(d)||d!==this._$AH[a]),d===k?e=k:e!==k&&(e+=(d??"")+s[a+1]),this._$AH[a]=d}o&&!n&&this.j(e)}j(e){e===k?this.element.removeAttribute(this.name):this.element.setAttribute(this.name,e??"")}},$r=class extends Ve{constructor(){super(...arguments),this.type=3}j(e){this.element[this.name]=e===k?void 0:e}};const _r=oe?oe.emptyScript:"";let Sr=class extends Ve{constructor(){super(...arguments),this.type=4}j(e){e&&e!==k?this.element.setAttribute(this.name,_r):this.element.removeAttribute(this.name)}},Er=class extends Ve{constructor(e,t,r,n,s){super(e,t,r,n,s),this.type=5}_$AI(e,t=this){var r;if((e=(r=ae(this,e,t,0))!==null&&r!==void 0?r:k)===we)return;const n=this._$AH,s=e===k&&n!==k||e.capture!==n.capture||e.once!==n.once||e.passive!==n.passive,o=e!==k&&(n===k||s);s&&this.element.removeEventListener(this.name,this,n),o&&this.element.addEventListener(this.name,this,e),this._$AH=e}handleEvent(e){var t,r;typeof this._$AH=="function"?this._$AH.call((r=(t=this.options)===null||t===void 0?void 0:t.host)!==null&&r!==void 0?r:this.element,e):this._$AH.handleEvent(e)}},Ar=class{constructor(e,t,r){this.element=e,this.type=6,this._$AN=void 0,this._$AM=t,this.options=r}get _$AU(){return this._$AM._$AU}_$AI(e){ae(this,e)}};const js={I:At},Zt=Be.litHtmlPolyfillSupport;Zt==null||Zt(gt,At),((Xe=Be.litHtmlVersions)!==null&&Xe!==void 0?Xe:Be.litHtmlVersions=[]).push("2.8.0");/**
 * @license
 * Copyright 2017 Google LLC
 * SPDX-License-Identifier: BSD-3-Clause
 */var et;const De=window,le=De.trustedTypes,Qt=le?le.createPolicy("lit-html",{createHTML:i=>i}):void 0,ft="$lit$",W=`lit$${(Math.random()+"").slice(9)}$`,Ci="?"+W,kr=`<${Ci}>`,J=document,$e=()=>J.createComment(""),_e=i=>i===null||typeof i!="object"&&typeof i!="function",xi=Array.isArray,Ir=i=>xi(i)||typeof(i==null?void 0:i[Symbol.iterator])=="function",tt=`[ 	
\f\r]`,fe=/<(?:(!--|\/[^a-zA-Z])|(\/?[a-zA-Z][^>\s]*)|(\/?$))/g,Xt=/-->/g,Jt=/>/g,K=RegExp(`>|${tt}(?:([^\\s"'>=/]+)(${tt}*=${tt}*(?:[^ 	
\f\r"'\`<>=]|("|')|))|$)`,"g"),ei=/'/g,ti=/"/g,Li=/^(?:script|style|textarea|title)$/i,Ti=i=>(e,...t)=>({_$litType$:i,strings:e,values:t}),p=Ti(1),Cr=Ti(2),ce=Symbol.for("lit-noChange"),y=Symbol.for("lit-nothing"),ii=new WeakMap,Z=J.createTreeWalker(J,129,null,!1);function Pi(i,e){if(!Array.isArray(i)||!i.hasOwnProperty("raw"))throw Error("invalid template strings array");return Qt!==void 0?Qt.createHTML(e):e}const xr=(i,e)=>{const t=i.length-1,r=[];let n,s=e===2?"<svg>":"",o=fe;for(let l=0;l<t;l++){const a=i[l];let d,u,m=-1,v=0;for(;v<a.length&&(o.lastIndex=v,u=o.exec(a),u!==null);)v=o.lastIndex,o===fe?u[1]==="!--"?o=Xt:u[1]!==void 0?o=Jt:u[2]!==void 0?(Li.test(u[2])&&(n=RegExp("</"+u[2],"g")),o=K):u[3]!==void 0&&(o=K):o===K?u[0]===">"?(o=n??fe,m=-1):u[1]===void 0?m=-2:(m=o.lastIndex-u[2].length,d=u[1],o=u[3]===void 0?K:u[3]==='"'?ti:ei):o===ti||o===ei?o=K:o===Xt||o===Jt?o=fe:(o=K,n=void 0);const b=o===K&&i[l+1].startsWith("/>")?" ":"";s+=o===fe?a+kr:m>=0?(r.push(d),a.slice(0,m)+ft+a.slice(m)+W+b):a+W+(m===-2?(r.push(void 0),l):b)}return[Pi(i,s+(i[t]||"<?>")+(e===2?"</svg>":"")),r]};class Se{constructor({strings:e,_$litType$:t},r){let n;this.parts=[];let s=0,o=0;const l=e.length-1,a=this.parts,[d,u]=xr(e,t);if(this.el=Se.createElement(d,r),Z.currentNode=this.el.content,t===2){const m=this.el.content,v=m.firstChild;v.remove(),m.append(...v.childNodes)}for(;(n=Z.nextNode())!==null&&a.length<l;){if(n.nodeType===1){if(n.hasAttributes()){const m=[];for(const v of n.getAttributeNames())if(v.endsWith(ft)||v.startsWith(W)){const b=u[o++];if(m.push(v),b!==void 0){const x=n.getAttribute(b.toLowerCase()+ft).split(W),C=/([.?@])?(.*)/.exec(b);a.push({type:1,index:s,name:C[2],strings:x,ctor:C[1]==="."?Tr:C[1]==="?"?Mr:C[1]==="@"?Rr:He})}else a.push({type:6,index:s})}for(const v of m)n.removeAttribute(v)}if(Li.test(n.tagName)){const m=n.textContent.split(W),v=m.length-1;if(v>0){n.textContent=le?le.emptyScript:"";for(let b=0;b<v;b++)n.append(m[b],$e()),Z.nextNode(),a.push({type:2,index:++s});n.append(m[v],$e())}}}else if(n.nodeType===8)if(n.data===Ci)a.push({type:2,index:s});else{let m=-1;for(;(m=n.data.indexOf(W,m+1))!==-1;)a.push({type:7,index:s}),m+=W.length-1}s++}}static createElement(e,t){const r=J.createElement("template");return r.innerHTML=e,r}}function de(i,e,t=i,r){var n,s,o,l;if(e===ce)return e;let a=r!==void 0?(n=t._$Co)===null||n===void 0?void 0:n[r]:t._$Cl;const d=_e(e)?void 0:e._$litDirective$;return(a==null?void 0:a.constructor)!==d&&((s=a==null?void 0:a._$AO)===null||s===void 0||s.call(a,!1),d===void 0?a=void 0:(a=new d(i),a._$AT(i,t,r)),r!==void 0?((o=(l=t)._$Co)!==null&&o!==void 0?o:l._$Co=[])[r]=a:t._$Cl=a),a!==void 0&&(e=de(i,a._$AS(i,e.values),a,r)),e}class Lr{constructor(e,t){this._$AV=[],this._$AN=void 0,this._$AD=e,this._$AM=t}get parentNode(){return this._$AM.parentNode}get _$AU(){return this._$AM._$AU}u(e){var t;const{el:{content:r},parts:n}=this._$AD,s=((t=e==null?void 0:e.creationScope)!==null&&t!==void 0?t:J).importNode(r,!0);Z.currentNode=s;let o=Z.nextNode(),l=0,a=0,d=n[0];for(;d!==void 0;){if(l===d.index){let u;d.type===2?u=new ke(o,o.nextSibling,this,e):d.type===1?u=new d.ctor(o,d.name,d.strings,this,e):d.type===6&&(u=new Br(o,this,e)),this._$AV.push(u),d=n[++a]}l!==(d==null?void 0:d.index)&&(o=Z.nextNode(),l++)}return Z.currentNode=J,s}v(e){let t=0;for(const r of this._$AV)r!==void 0&&(r.strings!==void 0?(r._$AI(e,r,t),t+=r.strings.length-2):r._$AI(e[t])),t++}}class ke{constructor(e,t,r,n){var s;this.type=2,this._$AH=y,this._$AN=void 0,this._$AA=e,this._$AB=t,this._$AM=r,this.options=n,this._$Cp=(s=n==null?void 0:n.isConnected)===null||s===void 0||s}get _$AU(){var e,t;return(t=(e=this._$AM)===null||e===void 0?void 0:e._$AU)!==null&&t!==void 0?t:this._$Cp}get parentNode(){let e=this._$AA.parentNode;const t=this._$AM;return t!==void 0&&(e==null?void 0:e.nodeType)===11&&(e=t.parentNode),e}get startNode(){return this._$AA}get endNode(){return this._$AB}_$AI(e,t=this){e=de(this,e,t),_e(e)?e===y||e==null||e===""?(this._$AH!==y&&this._$AR(),this._$AH=y):e!==this._$AH&&e!==ce&&this._(e):e._$litType$!==void 0?this.g(e):e.nodeType!==void 0?this.$(e):Ir(e)?this.T(e):this._(e)}k(e){return this._$AA.parentNode.insertBefore(e,this._$AB)}$(e){this._$AH!==e&&(this._$AR(),this._$AH=this.k(e))}_(e){this._$AH!==y&&_e(this._$AH)?this._$AA.nextSibling.data=e:this.$(J.createTextNode(e)),this._$AH=e}g(e){var t;const{values:r,_$litType$:n}=e,s=typeof n=="number"?this._$AC(e):(n.el===void 0&&(n.el=Se.createElement(Pi(n.h,n.h[0]),this.options)),n);if(((t=this._$AH)===null||t===void 0?void 0:t._$AD)===s)this._$AH.v(r);else{const o=new Lr(s,this),l=o.u(this.options);o.v(r),this.$(l),this._$AH=o}}_$AC(e){let t=ii.get(e.strings);return t===void 0&&ii.set(e.strings,t=new Se(e)),t}T(e){xi(this._$AH)||(this._$AH=[],this._$AR());const t=this._$AH;let r,n=0;for(const s of e)n===t.length?t.push(r=new ke(this.k($e()),this.k($e()),this,this.options)):r=t[n],r._$AI(s),n++;n<t.length&&(this._$AR(r&&r._$AB.nextSibling,n),t.length=n)}_$AR(e=this._$AA.nextSibling,t){var r;for((r=this._$AP)===null||r===void 0||r.call(this,!1,!0,t);e&&e!==this._$AB;){const n=e.nextSibling;e.remove(),e=n}}setConnected(e){var t;this._$AM===void 0&&(this._$Cp=e,(t=this._$AP)===null||t===void 0||t.call(this,e))}}class He{constructor(e,t,r,n,s){this.type=1,this._$AH=y,this._$AN=void 0,this.element=e,this.name=t,this._$AM=n,this.options=s,r.length>2||r[0]!==""||r[1]!==""?(this._$AH=Array(r.length-1).fill(new String),this.strings=r):this._$AH=y}get tagName(){return this.element.tagName}get _$AU(){return this._$AM._$AU}_$AI(e,t=this,r,n){const s=this.strings;let o=!1;if(s===void 0)e=de(this,e,t,0),o=!_e(e)||e!==this._$AH&&e!==ce,o&&(this._$AH=e);else{const l=e;let a,d;for(e=s[0],a=0;a<s.length-1;a++)d=de(this,l[r+a],t,a),d===ce&&(d=this._$AH[a]),o||(o=!_e(d)||d!==this._$AH[a]),d===y?e=y:e!==y&&(e+=(d??"")+s[a+1]),this._$AH[a]=d}o&&!n&&this.j(e)}j(e){e===y?this.element.removeAttribute(this.name):this.element.setAttribute(this.name,e??"")}}class Tr extends He{constructor(){super(...arguments),this.type=3}j(e){this.element[this.name]=e===y?void 0:e}}const Pr=le?le.emptyScript:"";class Mr extends He{constructor(){super(...arguments),this.type=4}j(e){e&&e!==y?this.element.setAttribute(this.name,Pr):this.element.removeAttribute(this.name)}}class Rr extends He{constructor(e,t,r,n,s){super(e,t,r,n,s),this.type=5}_$AI(e,t=this){var r;if((e=(r=de(this,e,t,0))!==null&&r!==void 0?r:y)===ce)return;const n=this._$AH,s=e===y&&n!==y||e.capture!==n.capture||e.once!==n.once||e.passive!==n.passive,o=e!==y&&(n===y||s);s&&this.element.removeEventListener(this.name,this,n),o&&this.element.addEventListener(this.name,this,e),this._$AH=e}handleEvent(e){var t,r;typeof this._$AH=="function"?this._$AH.call((r=(t=this.options)===null||t===void 0?void 0:t.host)!==null&&r!==void 0?r:this.element,e):this._$AH.handleEvent(e)}}class Br{constructor(e,t,r){this.element=e,this.type=6,this._$AN=void 0,this._$AM=t,this.options=r}get _$AU(){return this._$AM._$AU}_$AI(e){de(this,e)}}const ri=De.litHtmlPolyfillSupport;ri==null||ri(Se,ke),((et=De.litHtmlVersions)!==null&&et!==void 0?et:De.litHtmlVersions=[]).push("2.8.0");const Or=(i,e,t)=>{var r,n;const s=(r=t==null?void 0:t.renderBefore)!==null&&r!==void 0?r:e;let o=s._$litPart$;if(o===void 0){const l=(n=t==null?void 0:t.renderBefore)!==null&&n!==void 0?n:null;s._$litPart$=o=new ke(e.insertBefore($e(),l),l,void 0,t??{})}return o._$AI(i),o};/**
 * @license
 * Copyright 2017 Google LLC
 * SPDX-License-Identifier: BSD-3-Clause
 */var it,rt;class P extends se{constructor(){super(...arguments),this.renderOptions={host:this},this._$Do=void 0}createRenderRoot(){var e,t;const r=super.createRenderRoot();return(e=(t=this.renderOptions).renderBefore)!==null&&e!==void 0||(t.renderBefore=r.firstChild),r}update(e){const t=this.render();this.hasUpdated||(this.renderOptions.isConnected=this.isConnected),super.update(e),this._$Do=Or(t,this.renderRoot,this.renderOptions)}connectedCallback(){var e;super.connectedCallback(),(e=this._$Do)===null||e===void 0||e.setConnected(!0)}disconnectedCallback(){var e;super.disconnectedCallback(),(e=this._$Do)===null||e===void 0||e.setConnected(!1)}render(){return ce}}P.finalized=!0,P._$litElement$=!0,(it=globalThis.litElementHydrateSupport)===null||it===void 0||it.call(globalThis,{LitElement:P});const ni=globalThis.litElementPolyfillSupport;ni==null||ni({LitElement:P});((rt=globalThis.litElementVersions)!==null&&rt!==void 0?rt:globalThis.litElementVersions=[]).push("3.3.3");/**
 * @license
 * Copyright 2017 Google LLC
 * SPDX-License-Identifier: BSD-3-Clause
 */const A=i=>e=>typeof e=="function"?((t,r)=>(customElements.define(t,r),r))(i,e):((t,r)=>{const{kind:n,elements:s}=r;return{kind:n,elements:s,finisher(o){customElements.define(t,o)}}})(i,e);/**
 * @license
 * Copyright 2017 Google LLC
 * SPDX-License-Identifier: BSD-3-Clause
 */const Dr=(i,e)=>e.kind==="method"&&e.descriptor&&!("value"in e.descriptor)?{...e,finisher(t){t.createProperty(e.key,i)}}:{kind:"field",key:Symbol(),placement:"own",descriptor:{},originalKey:e.key,initializer(){typeof e.initializer=="function"&&(this[e.key]=e.initializer.call(this))},finisher(t){t.createProperty(e.key,i)}},Ur=(i,e,t)=>{e.constructor.createProperty(t,i)};function h(i){return(e,t)=>t!==void 0?Ur(i,e,t):Dr(i,e)}/**
 * @license
 * Copyright 2017 Google LLC
 * SPDX-License-Identifier: BSD-3-Clause
 */function S(i){return h({...i,state:!0})}/**
 * @license
 * Copyright 2017 Google LLC
 * SPDX-License-Identifier: BSD-3-Clause
 */const kt=({finisher:i,descriptor:e})=>(t,r)=>{var n;if(r===void 0){const s=(n=t.originalKey)!==null&&n!==void 0?n:t.key,o=e!=null?{kind:"method",placement:"prototype",key:s,descriptor:e(t.key)}:{...t,key:s};return i!=null&&(o.finisher=function(l){i(l,s)}),o}{const s=t.constructor;e!==void 0&&Object.defineProperty(t,r,e(r)),i==null||i(s,r)}};/**
 * @license
 * Copyright 2017 Google LLC
 * SPDX-License-Identifier: BSD-3-Clause
 */function re(i,e){return kt({descriptor:t=>({get(){var n,s;return(s=(n=this.renderRoot)===null||n===void 0?void 0:n.querySelector(i))!==null&&s!==void 0?s:null},enumerable:!0,configurable:!0})})}/**
 * @license
 * Copyright 2017 Google LLC
 * SPDX-License-Identifier: BSD-3-Clause
 */function Nr(i){return kt({descriptor:e=>({get(){var t,r;return(r=(t=this.renderRoot)===null||t===void 0?void 0:t.querySelectorAll(i))!==null&&r!==void 0?r:[]},enumerable:!0,configurable:!0})})}/**
 * @license
 * Copyright 2021 Google LLC
 * SPDX-License-Identifier: BSD-3-Clause
 */var nt;const zr=((nt=window.HTMLSlotElement)===null||nt===void 0?void 0:nt.prototype.assignedElements)!=null?(i,e)=>i.assignedElements(e):(i,e)=>i.assignedNodes(e).filter((t=>t.nodeType===Node.ELEMENT_NODE));function qs(i){const{slot:e,selector:t}=i??{};return kt({descriptor:r=>({get(){var n;const s="slot"+(e?`[name=${e}]`:":not([name])"),o=(n=this.renderRoot)===null||n===void 0?void 0:n.querySelector(s),l=o!=null?zr(o,i):[];return t?l.filter((a=>a.matches(t))):l},enumerable:!0,configurable:!0})})}function c(i,e,t,r){var n=arguments.length,s=n<3?e:r===null?r=Object.getOwnPropertyDescriptor(e,t):r,o;if(typeof Reflect=="object"&&typeof Reflect.decorate=="function")s=Reflect.decorate(i,e,t,r);else for(var l=i.length-1;l>=0;l--)(o=i[l])&&(s=(n<3?o(s):n>3?o(e,t,s):o(e,t))||s);return n>3&&s&&Object.defineProperty(e,t,s),s}function Ks(i,e){if(typeof Reflect=="object"&&typeof Reflect.metadata=="function")return Reflect.metadata(i,e)}function si(i,e,t,r){function n(s){return s instanceof t?s:new t(function(o){o(s)})}return new(t||(t=Promise))(function(s,o){function l(u){try{d(r.next(u))}catch(m){o(m)}}function a(u){try{d(r.throw(u))}catch(m){o(m)}}function d(u){u.done?s(u.value):n(u.value).then(l,a)}d((r=r.apply(i,e||[])).next())})}const T={eventCategory:"TopNav",hiddenSearchOptions:[],waybackPagesArchived:"1 trillion"};function yt(i="___USERID___",e="https://archive.org",t="",r=""){return t&&(T.waybackPagesArchived=t),{audio:{heading:"Internet Archive Audio",iconLinks:[{icon:`${e}/services/img/etree`,title:"Live Music Archive",url:`${e}/details/etree`},{icon:`${e}/services/img/librivoxaudio`,title:"Librivox Free Audio",url:`${e}/details/librivoxaudio`}],featuredLinks:[{title:"All Audio",url:`${e}/details/audio`},{title:"Grateful Dead",url:`${e}/details/GratefulDead`},{title:"Netlabels",url:`${e}/details/netlabels`},{title:"Old Time Radio",url:`${e}/details/oldtimeradio`},{title:"78 RPMs and Cylinder Recordings",url:`${e}/details/78rpm`}],links:[{title:"Audio Books & Poetry",url:`${e}/details/audio_bookspoetry`},{title:"Computers, Technology and Science",url:`${e}/details/audio_tech`},{title:"Music, Arts & Culture",url:`${e}/details/audio_music`},{title:"News & Public Affairs",url:`${e}/details/audio_news`},{title:"Spirituality & Religion",url:`${e}/details/audio_religion`},{title:"Podcasts",url:`${e}/details/podcasts`},{title:"Radio News Archive",url:`${e}/details/radio`}],mobileAppsLinks:[],browserExtensionsLinks:[],archiveItLinks:[]},images:{heading:"Images",iconLinks:[{icon:`${e}/services/img/metropolitanmuseumofart-gallery`,title:"Metropolitan Museum",url:`${e}/details/metropolitanmuseumofart-gallery`},{icon:`${e}/services/img/clevelandart`,title:"Cleveland Museum of Art",url:`${e}/details/clevelandart`}],featuredLinks:[{title:"All Images",url:`${e}/details/image`},{title:"Flickr Commons",url:`${e}/details/flickrcommons`},{title:"Occupy Wall Street Flickr",url:`${e}/details/flickr-ows`},{title:"Cover Art",url:`${e}/details/coverartarchive`},{title:"USGS Maps",url:`${e}/details/maps_usgs`}],links:[{title:"NASA Images",url:`${e}/details/nasa`},{title:"Solar System Collection",url:`${e}/details/solarsystemcollection`},{title:"Ames Research Center",url:`${e}/details/amesresearchcenterimagelibrary`}],mobileAppsLinks:[],browserExtensionsLinks:[],archiveItLinks:[]},more:{links:[{title:"About",url:`${e}/about/`},{title:"Blog",url:"https://blog.archive.org"},{title:"Events",url:`${e}/events`},{title:"Projects",url:`${e}/projects/`},{title:"Help",url:`${e}/about/faqs.php`},{title:"Donate",url:`${e}/donate?origin=iawww-TopNavDonateButton`},{title:"Contact",url:`${e}/about/contact`},{title:"Jobs",url:`${e}/about/jobs`},{title:"Volunteer",url:`${e}/about/volunteer-positions`}],heading:"",iconLinks:[],featuredLinks:[],mobileAppsLinks:[],browserExtensionsLinks:[],archiveItLinks:[]},software:{heading:"Software",iconLinks:[{icon:`${e}/services/img/internetarcade`,title:"Internet Arcade",url:`${e}/details/internetarcade`},{icon:`${e}/services/img/consolelivingroom`,title:"Console Living Room",url:`${e}/details/consolelivingroom`}],featuredLinks:[{title:"All Software",url:`${e}/details/software`},{title:"Old School Emulation",url:`${e}/details/tosec`},{title:"MS-DOS Games",url:`${e}/details/softwarelibrary_msdos_games`},{title:"Historical Software",url:`${e}/details/historicalsoftware`},{title:"Classic PC Games",url:`${e}/details/classicpcgames`},{title:"Software Library",url:`${e}/details/softwarelibrary`}],links:[{title:"Kodi Archive and Support File",url:`${e}/details/kodi_archive`},{title:"Vintage Software",url:`${e}/details/vintagesoftware`},{title:"APK",url:`${e}/details/apkarchive`},{title:"MS-DOS",url:`${e}/details/softwarelibrary_msdos`},{title:"CD-ROM Software",url:`${e}/details/cd-roms`},{title:"CD-ROM Software Library",url:`${e}/details/cdromsoftware`},{title:"Software Sites",url:`${e}/details/softwaresites`},{title:"Tucows Software Library",url:`${e}/details/tucows`},{title:"Shareware CD-ROMs",url:`${e}/details/cdbbsarchive`},{title:"Software Capsules Compilation",url:`${e}/details/softwarecapsules`},{title:"CD-ROM Images",url:`${e}/details/cdromimages`},{title:"ZX Spectrum",url:`${e}/details/softwarelibrary_zx_spectrum`},{title:"DOOM Level CD",url:`${e}/details/doom-cds`}],mobileAppsLinks:[],browserExtensionsLinks:[],archiveItLinks:[]},texts:{heading:"Texts",iconLinks:[{title:"Open Library",icon:`${e}/images/widgetOL.png`,url:"https://openlibrary.org/"},{title:"American Libraries",icon:`${e}/services/img/americana`,url:`${e}/details/americana`}],featuredLinks:[{title:"All Texts",url:`${e}/details/texts`},{title:"Smithsonian Libraries",url:`${e}/details/smithsonian`},{title:"FEDLINK (US)",url:`${e}/details/fedlink`},{title:"Genealogy",url:`${e}/details/genealogy`},{title:"Lincoln Collection",url:`${e}/details/lincolncollection`}],links:[{title:"American Libraries",url:`${e}/details/americana`},{title:"Canadian Libraries",url:`${e}/details/toronto`},{title:"Universal Library",url:`${e}/details/universallibrary`},{title:"Project Gutenberg",url:`${e}/details/gutenberg`},{title:"Children's Library",url:`${e}/details/iacl`},{title:"Biodiversity Heritage Library",url:`${e}/details/biodiversity`},{title:"Books by Language",url:`${e}/details/booksbylanguage`},{title:"Additional Collections",url:`${e}/details/additional_collections`}],mobileAppsLinks:[],browserExtensionsLinks:[],archiveItLinks:[]},web:{mobileAppsLinks:[{url:"https://apps.apple.com/us/app/wayback-machine/id1201888313",title:"Wayback Machine (iOS)",external:!0},{url:"https://play.google.com/store/apps/details?id=com.internetarchive.waybackmachine",title:"Wayback Machine (Android)",external:!0}],browserExtensionsLinks:[{url:"https://chrome.google.com/webstore/detail/wayback-machine/fpnmgdkabkmnadcjpehmlllkndpkmiak",title:"Chrome",external:!0},{url:"https://addons.mozilla.org/en-US/firefox/addon/wayback-machine_new/",title:"Firefox",external:!0},{url:"https://apps.apple.com/us/app/wayback-machine/id1472432422?mt=12",title:"Safari",external:!0},{url:"https://microsoftedge.microsoft.com/addons/detail/wayback-machine/kjmickeoogghaimmomagaghnogelpcpn?hl=en-US",title:"Edge",external:!0}],archiveItLinks:[{url:"https://www.archive-it.org/explore",title:"Explore the Collections",external:!0},{url:"https://www.archive-it.org/blog/learn-more/",title:"Learn More",external:!0},{url:"https://www.archive-it.org/contact-us",title:"Build Collections",external:!0}],heading:"",iconLinks:[],featuredLinks:[],links:[]},video:{heading:"Video",iconLinks:[{icon:`${e}/services/img/tv`,title:"TV News",url:`${e}/details/tv`},{icon:`${e}/services/img/911`,title:"Understanding 9/11",url:`${e}/details/911`}],featuredLinks:[{title:"All Video",url:`${e}/details/movies`},{title:"Prelinger Archives",url:`${e}/details/prelinger`},{title:"Democracy Now!",url:`${e}/details/democracy_now_vid`},{title:"Occupy Wall Street",url:`${e}/details/occupywallstreet`},{title:"TV NSA Clip Library",url:`${e}/details/nsa`}],links:[{title:"Animation & Cartoons",url:`${e}/details/animationandcartoons`},{title:"Arts & Music",url:`${e}/details/artsandmusicvideos`},{title:"Computers & Technology",url:`${e}/details/computersandtechvideos`},{title:"Cultural & Academic Films",url:`${e}/details/culturalandacademicfilms`},{title:"Ephemeral Films",url:`${e}/details/ephemera`},{title:"Movies",url:`${e}/details/moviesandfilms`},{title:"News & Public Affairs",url:`${e}/details/newsandpublicaffairs`},{title:"Spirituality & Religion",url:`${e}/details/spiritualityandreligion`},{title:"Sports Videos",url:`${e}/details/sports`},{title:"Television",url:`${e}/details/television`},{title:"Videogame Videos",url:`${e}/details/gamevideos`},{title:"Vlogs",url:`${e}/details/vlogs`},{title:"Youth Media",url:`${e}/details/youth_media`}],mobileAppsLinks:[],browserExtensionsLinks:[],archiveItLinks:[]},user:[{url:`${e}/upload`,title:"Upload files",analyticsEvent:"UserUpload",class:"mobile-upload"},{url:`${e}/details/@${i}`,title:"My uploads",analyticsEvent:"UserLibrary"},{url:`${e}/details/@${i}/loans`,title:"My loans",analyticsEvent:"UserLoans"},{url:`${e}/details/fav-${i}`,title:"My favorites",analyticsEvent:"UserFavorites"},{url:`${e}/details/@${i}/lists`,title:"My lists",analyticsEvent:"UserLists"},{url:`${e}/details/@${i}/collections`,title:"My collections",analyticsEvent:"UserCollections"},{url:`${e}/details/@${i}/web-archive`,title:"My web archives",analyticsEvent:"UserWebArchive"},{url:`${e}/account/settings`,title:"Account settings",analyticsEvent:"UserSettings"},{url:"https://help.archive.org",title:"Get help",analyticsEvent:"UserHelp"},{url:`${e}/account/logout`,title:"Log out",analyticsEvent:"UserLogOut"}],userAdmin:[{title:"ADMINS:"},{title:"item:"},{url:`${e}/editxml/${r}`,title:"edit xml",analyticsEvent:"AdminUserEditXML"},{url:`${e}/edit.php?redir=1&identifier=${r}`,title:"edit files",analyticsEvent:"AdminUserEditFiles"},{url:`${e}/download/${r}/`,title:"download",analyticsEvent:"AdminUserDownload"},{url:`${e}/metadata/${r}/`,title:"metadata",analyticsEvent:"AdminUserMetadata"},{url:`https://catalogd.archive.org/history/${r}`,title:"history",analyticsEvent:"AdminUserHistory"},{url:`${e}/manage/${r}`,title:"manage",analyticsEvent:"AdminUserManager"},{url:`${e}/manage/${r}#make_dark`,title:"curate",analyticsEvent:"AdminUserCurate"},{url:`${e}/manage/${r}#modify_xml`,title:"modify xml",analyticsEvent:"AdminUserModifyXML"}],userAdminFlags:[{url:`${e}/services/flags/admin.php?identifier=${r}`,title:"manage flags",analyticsEvent:"AdminUserManageFlags"}],signedOut:[{url:`${e}/account/signup`,title:"Sign up for free",analyticsEvent:"AvatarMenu-Signup"},{url:`${e}/login`,title:"Log in",analyticsEvent:"AvatarMenu-Login"}]}}class B extends P{trackClick(e){var t,r;const n=(r=(t=e.currentTarget)===null||t===void 0?void 0:t.dataset)===null||r===void 0?void 0:r.eventClickTracking;n!==void 0&&this.dispatchEvent(new CustomEvent("trackClick",{bubbles:!0,composed:!0,detail:{event:n}}))}trackSubmit(e){var t,r;const n=(r=(t=e.currentTarget)===null||t===void 0?void 0:t.dataset)===null||r===void 0?void 0:r.eventSubmitTracking;n!==void 0&&this.dispatchEvent(new CustomEvent("trackSubmit",{bubbles:!0,composed:!0,detail:{event:n}}))}}const Vr=g`
  ul {
    position: relative;
    z-index: 3;
    padding: 0.8rem 0;
    margin: 0;
    font-size: 1.2rem;
    text-transform: uppercase;
    text-align: center;
    background: var(--desktopSubnavBg);
  }

  li {
    display: inline-block;
    padding: 0 15px;
  }

  a {
    text-decoration: none;
    color: var(--subnavLinkColor);
    outline: none;
  }

  a:hover,
  a:active,
  a:focus {
    color: var(--linkHoverColor);
  }

  .donate svg {
    width: 1.6rem;
    height: 1.6rem;
    vertical-align: top;
    fill: #f00;
  }
`,Hr=p`
  <svg
    width="40px"
    height="40px"
    viewBox="0 0 40 40"
    version="1.1"
    xmlns="http://www.w3.org/2000/svg"
    aria-labelledby="audioTitleID audioDescID"
  >
    <title id="audioTitleID">Audio icon</title>
    <desc id="audioDescID">An illustration of an audio speaker.</desc>
    <g stroke="none" stroke-width="1" fill="none" fill-rule="evenodd">
      <g transform="translate(10, 8)" class="fill-color">
        <path
          d="M19.4264564,11.8585048 L19.4264564,20.7200433 C19.4264564,22.3657576 18.8838179,23.2519114 16.8489237,23.2519114 C12.2364969,23.125318 7.75972977,23.125318 3.14730298,23.2519114 C1.24806842,23.2519114 0.569770368,22.492351 0.569770368,20.7200433 L0.569770368,2.74377955 C0.569770368,1.09806526 1.11240881,0.211911416 3.14730298,0.211911416 C7.75972977,0.338504822 12.2364969,0.338504822 16.8489237,0.211911416 C18.7481583,0.211911416 19.4264564,0.971471855 19.4264564,2.74377955 C19.2907967,5.78202131 19.4264564,8.82026306 19.4264564,11.8585048 L19.4264564,11.8585048 Z M10.0659432,2.74377955 C8.16670861,2.74377955 6.67445288,4.13630702 6.67445288,5.90861471 C6.67445288,7.6809224 8.16670861,9.07344988 10.0659432,9.07344988 C11.9651777,9.07344988 13.4574335,7.6809224 13.4574335,5.90861471 C13.4574335,4.13630702 11.8295181,2.74377955 10.0659432,2.74377955 L10.0659432,2.74377955 Z M10.0659432,11.4787246 C7.21709133,11.4787246 5.04653754,13.6308125 5.04653754,16.1626806 C5.04653754,18.8211422 7.35275094,20.8466367 10.0659432,20.8466367 C12.914795,20.8466367 15.0853488,18.6945488 15.0853488,16.1626806 C15.0853488,13.6308125 12.7791354,11.4787246 10.0659432,11.4787246 L10.0659432,11.4787246 Z"
        ></path>
        <ellipse
          cx="10.2016028"
          cy="16.5690777"
          rx="1.35659611"
          ry="1.34075134"
        ></ellipse>
      </g>
    </g>
  </svg>
`,Fr=p`
  <svg
    width="40"
    height="40"
    viewBox="0 0 40 40"
    version="1.1"
    xmlns="http://www.w3.org/2000/svg"
    aria-labelledby="closeTitleID closeDescID"
  >
    <title id="closeTitleID">Close icon</title>
    <desc id="closeDescID">A line drawing of an X</desc>
    <path
      d="m29.1923882 10.8076118c.5857864.5857865.5857864 1.535534 0 2.1213204l-7.0711162 7.0703398 7.0711162 7.0717958c.5857864.5857864.5857864 1.5355339 0 2.1213204-.5857865.5857864-1.535534.5857864-2.1213204 0l-7.0717958-7.0711162-7.0703398 7.0711162c-.5857864.5857864-1.5355339.5857864-2.1213204 0-.5857864-.5857865-.5857864-1.535534 0-2.1213204l7.0706602-7.0717958-7.0706602-7.0703398c-.5857864-.5857864-.5857864-1.5355339 0-2.1213204.5857865-.5857864 1.535534-.5857864 2.1213204 0l7.0703398 7.0706602 7.0717958-7.0706602c.5857864-.5857864 1.5355339-.5857864 2.1213204 0z"
      class="fill-color"
      fill-rule="evenodd"
    />
  </svg>
`,Wr=p`
  <svg
    width="40"
    height="40"
    viewBox="0 0 40 40"
    xmlns="http://www.w3.org/2000/svg"
    aria-labelledby="donateTitleID donateDescID"
  >
    <title id="donateTitleID">Donate icon</title>
    <desc id="donateDescID">An illustration of a heart shape</desc>
    <path
      class="fill-color"
      d="m30.0120362 11.0857287c-1.2990268-1.12627221-2.8599641-1.65258786-4.682812-1.57894699-.8253588.02475323-1.7674318.3849128-2.8262192 1.08047869-1.0587873.6955659-1.89622 1.5724492-2.512298 2.63065-.591311-1.0588196-1.4194561-1.9357029-2.4844351-2.63065-1.0649791-.69494706-2.0039563-1.05510663-2.8169316-1.08047869-1.2067699-.04950647-2.318187.17203498-3.3342513.66462439-1.0160643.4925893-1.82594378 1.2002224-2.42963831 2.1228992-.60369453.9226769-.91173353 1.9629315-.92411701 3.1207641-.03715043 1.9202322.70183359 3.7665141 2.21695202 5.5388457 1.2067699 1.4035084 2.912594 3.1606786 5.1174721 5.2715107 2.2048782 2.1108321 3.7565279 3.5356901 4.6549492 4.2745742.8253588-.6646243 2.355647-2.0647292 4.5908647-4.2003145s3.9747867-3.9171994 5.218707-5.3448422c1.502735-1.7723316 2.2355273-3.6186135 2.1983769-5.5388457-.0256957-1.7608832-.6875926-3.2039968-1.9866194-4.3302689z"
    />
  </svg>
`,jr=p`
  <svg
    width="40"
    height="40"
    viewBox="0 0 40 40"
    xmlns="http://www.w3.org/2000/svg"
    xmlns:svg="http://www.w3.org/2000/svg"
    aria-labelledby="donateTitleID donateDescID"
  >
    <title id="donateTitleID">Donate icon</title>
    <desc id="donateDescID">An illustration of a heart shape</desc>
    <path
      class="fill-color"
      d="m32.6,8.12c-1.57,-1.51 -3.46,-2.21 -5.65,-2.11c-1,0.03 -2.14,0.51 -3.42,1.44c-1.28,0.92 -2.28,2.09 -3.03,3.51c-0.71,-1.41 -1.72,-2.59 -3,-3.51c-1.29,-0.93 -2.43,-1.41 -3.41,-1.44c-1.46,-0.07 -2.8,0.23 -4.02,0.88c-1.23,0.65 -2.21,1.6 -2.94,2.83c-0.74,1.24 -1.11,2.63 -1.12,4.17c-0.05,2.56 0.85,5.01 2.68,7.37c1.46,1.88 3.52,4.21 6.19,7.04c2.66,2.81 4.53,4.71 5.62,5.69c1,-0.88 2.85,-2.75 5.55,-5.6s4.81,-5.23 6.31,-7.13c1.81,-2.36 2.71,-4.81 2.66,-7.37c-0.04,-2.35 -0.83,-4.28 -2.4,-5.77z"
    />
  </svg>
`,qr=p`
  <svg
    height="40"
    viewBox="0 0 40 40"
    width="40"
    xmlns="http://www.w3.org/2000/svg"
    aria-labelledby="ellipsesTitleID ellipsesDescID"
  >
    <title id="ellipsesTitleID">Ellipses icon</title>
    <desc id="ellipsesDescID">An illustration of text ellipses.</desc>
    <path
      class="fill-color"
      d="m10.5 17.5c1.3807119 0 2.5 1.1192881 2.5 2.5s-1.1192881 2.5-2.5 2.5c-1.38071187 0-2.5-1.1192881-2.5-2.5s1.11928813-2.5 2.5-2.5zm9.5 0c1.3807119 0 2.5 1.1192881 2.5 2.5s-1.1192881 2.5-2.5 2.5-2.5-1.1192881-2.5-2.5 1.1192881-2.5 2.5-2.5zm9.5 0c1.3807119 0 2.5 1.1192881 2.5 2.5s-1.1192881 2.5-2.5 2.5-2.5-1.1192881-2.5-2.5 1.1192881-2.5 2.5-2.5z"
      fill-rule="evenodd"
    />
  </svg>
`,Kr=p`
  <svg
    class="ia-logo"
    viewBox="0 0 27 30"
    xmlns="http://www.w3.org/2000/svg"
    aria-labelledby="logoTitleID logoDescID"
  >
    <title id="logoTitleID">Internet Archive logo</title>
    <desc id="logoDescID">
      A line drawing of the Internet Archive headquarters building façade.
    </desc>
    <g stroke="none" stroke-width="1" fill="none" fill-rule="evenodd">
      <mask id="mask-2" fill="white">
        <path
          d="M26.6666667,28.6046512 L26.6666667,30 L0,30 L0.000283687943,28.6046512 L26.6666667,28.6046512 Z M25.6140351,26.5116279 L25.6140351,28.255814 L1.05263158,28.255814 L1.05263158,26.5116279 L25.6140351,26.5116279 Z M3.62469203,7.6744186 L3.91746909,7.82153285 L4.0639977,10.1739544 L4.21052632,13.9963932 L4.21052632,17.6725617 L4.0639977,22.255044 L4.03962296,25.3421929 L3.62469203,25.4651163 L2.16024641,25.4651163 L1.72094074,25.3421929 L1.55031755,22.255044 L1.40350877,17.6970339 L1.40350877,14.0211467 L1.55031755,10.1739544 L1.68423854,7.80887484 L1.98962322,7.6744186 L3.62469203,7.6744186 Z M24.6774869,7.6744186 L24.9706026,7.82153285 L25.1168803,10.1739544 L25.2631579,13.9963932 L25.2631579,17.6725617 L25.1168803,22.255044 L25.0927809,25.3421929 L24.6774869,25.4651163 L23.2130291,25.4651163 L22.7736357,25.3421929 L22.602418,22.255044 L22.4561404,17.6970339 L22.4561404,14.0211467 L22.602418,10.1739544 L22.7369262,7.80887484 L23.0420916,7.6744186 L24.6774869,7.6744186 Z M9.94042303,7.6744186 L10.2332293,7.82153285 L10.3797725,10.1739544 L10.5263158,13.9963932 L10.5263158,17.6725617 L10.3797725,22.255044 L10.3556756,25.3421929 L9.94042303,25.4651163 L8.47583122,25.4651163 L8.0362015,25.3421929 L7.86556129,22.255044 L7.71929825,17.6970339 L7.71929825,14.0211467 L7.86556129,10.1739544 L8.00005604,7.80887484 L8.30491081,7.6744186 L9.94042303,7.6744186 Z M18.0105985,7.6744186 L18.3034047,7.82153285 L18.449948,10.1739544 L18.5964912,13.9963932 L18.5964912,17.6725617 L18.449948,22.255044 L18.425851,25.3421929 L18.0105985,25.4651163 L16.5460067,25.4651163 L16.1066571,25.3421929 L15.9357367,22.255044 L15.7894737,17.6970339 L15.7894737,14.0211467 L15.9357367,10.1739544 L16.0702315,7.80887484 L16.3753664,7.6744186 L18.0105985,7.6744186 Z M25.6140351,4.53488372 L25.6140351,6.97674419 L1.05263158,6.97674419 L1.05263158,4.53488372 L25.6140351,4.53488372 Z M13.0806755,0 L25.9649123,2.93331338 L25.4484139,3.8372093 L0.771925248,3.8372093 L0,3.1041615 L13.0806755,0 Z"
          id="path-1"
        ></path>
      </mask>
      <use fill="#FFFFFF" xlink:href="#path-1"></use>
      <g mask="url(#mask-2)" fill="#FFFFFF">
        <path
          d="M0,0 L26.6666667,0 L26.6666667,30 L0,30 L0,0 Z"
          id="swatch"
        ></path>
      </g>
    </g>
  </svg>
`,Gr=p`
  <svg
    height="40"
    viewBox="0 0 40 40"
    width="40"
    xmlns="http://www.w3.org/2000/svg"
    aria-labelledby="imagesTitleID imagesDescID"
  >
    <title id="imagesTitleID">Images icon</title>
    <desc id="imagesDescID">An illustration of two photographs.</desc>
    <path
      class="fill-color"
      d="m20.8219178 15.3769871c0 1.1136708-.8767123 1.8932404-1.8630137 1.8932404s-1.9726027-.8909367-1.9726027-1.8932404c0-1.0023038.8767123-1.8932404 1.9726027-1.8932404.9863014 0 1.8630137.8909366 1.8630137 1.8932404zm-5.9178082-3.7864808h15.4520548v6.0138225l-1.9726028-3.3410125-2.6301369 6.3479237-2.1917809-2.67281-6.1369863 5.1228859h-2.5205479zm-1.7534247-1.6705063v14.9231892h18.8493151v-14.9231892zm-2.9589041 7.2388604c.2191781 0 1.9726028-.3341012 1.9726028-.3341012v-2.0046075l-4.1643836.5568354c.43835616 4.7887846.87671233 9.9116704 1.31506849 14.700455 6.02739731-.5568354 13.26027401-1.5591391 19.39726031-2.1159746-.1095891-.5568354-.1095891-2.0046075-.2191781-2.67281-.4383562.1113671-1.4246575 0-1.8630137.1113671v.8909367c-5.1506849.4454683-10.3013699 1.1136708-15.4520548 1.6705062.109589-.111367-.5479452-7.0161262-.9863014-10.8026071z"
      fill-rule="evenodd"
    />
  </svg>
`,Yr=p`
  <svg
    height="40"
    viewBox="0 0 40 40"
    width="40"
    xmlns="http://www.w3.org/2000/svg"
    aria-labelledby="searchTitleID searchDescID"
  >
    <title id="searchTitleID">Search icon</title>
    <desc id="searchDescID">An illustration of a magnifying glass.</desc>
    <path
      class="fill-color"
      d="m32.4526364 29.8875889-8.1719472-7.9751279c1.1046135-1.4876138 1.7652549-3.3102407 1.7652549-5.2846451 0-.101185-.0142895-.1981539-.030573-.2944743.0166158-.0976175.0309053-.196208.0309053-.2990145 0-4.9814145-4.152935-9.0343271-9.2572866-9.0343271-.0907218 0-.1781206.01394537-.2655193.02594487-.0880633-.0119995-.1747974-.02594487-.2655193-.02594487-5.1046839 0-9.25761889 4.0529126-9.25761889 9.0343271 0 .1011849.01395722.1981539.03057294.2947985-.01694804.0976176-.03090525.1958838-.03090525.2986903 0 4.9814145 4.1526027 9.0346514 9.2572866 9.0346514.0907218 0 .1777882-.0139454.2658516-.0262692.0873987.0123238.1741328.0262692.265187.0262692 1.7306942 0 3.3467399-.4747911 4.7338208-1.2852439l8.2882574 8.0886366c.3652137.3564177.843082.53414 1.3212826.53414.4782007 0 .9567336-.1780467 1.3212827-.53414.7294304-.7118622.7294304-1.8660845-.0003323-2.5782711zm-15.9526364-7.8875889c-.0832667-.0118703-.1652765-.0253024-.2513711-.0253024-2.8781993 0-5.2197212-2.3278242-5.2197212-5.1891862 0-.0974612-.013197-.1908615-.0289077-.2836371.0160249-.0940251.0292219-.1889874.0292219-.2880105 0-2.861362 2.3418361-5.1891861 5.2200354-5.1891861.0854662 0 .1677902-.0131198.2510569-.0246777.0826383.0115579.1649623.0246777.2510569.0246777 2.8781993 0 5.2197212 2.3278241 5.2197212 5.1891861 0 .0974612.0135112.1908616.0289077.2839496-.0157107.0940251-.0295361.1886749-.0295361.287698 0 2.861362-2.3415219 5.1891862-5.2197212 5.1891862-.0860946 0-.1684187.0134321-.2507427.0253024z"
      fill-rule="evenodd"
    />
  </svg>
`,Zr=p`
  <svg
    width="40"
    height="40"
    viewBox="0 0 40 40"
    xmlns="http://www.w3.org/2000/svg"
    aria-labelledby="softwareTitleID softwareDescID"
  >
    <title id="softwareTitleID">Software icon</title>
    <desc id="softwareDescID">An illustration of a 3.5" floppy disk.</desc>
    <path
      class="fill-color"
      d="m32 30.6900373v-21.44521088c0-.82988428-.4156786-1.24482642-1.2470357-1.24482642h-21.50592858c-.83135715 0-1.24703572.4221795-1.24703572 1.26653851v21.44521089c0 .8588337.41567857 1.2882506 1.24703572 1.2882506h21.48327168c.8458575 0 1.2687863-.4366542 1.2687863-1.3099627zm-5.9950155-20.4410268v6.114667c0 .6694561-.3428744 1.0041841-1.0286232 1.0041841h-10.1294464c-.2622159 0-.4773054-.0802141-.6452685-.2406423s-.2519447-.3642806-.2519447-.6115572v-6.1363791l.0217506-.1311772h12.0326259zm-4.9437353.8295827v5.0010178h3.0405558v-5.0010178zm-9.7134658 18.8035735v-7.753025c0-.5241057.1604108-.9025595.4812325-1.1353613.1897138-.1453504.4011782-.2180256.6343932-.2180256h14.7451099c.3208217 0 .5905898.1091636.8093044.3274907s.3280719.5023936.3280719.8521995v7.8181612l-.0217506.1094652h-16.9772676z"
    />
  </svg>
`,Qr=p`
  <svg
    height="40"
    viewBox="0 0 40 40"
    width="40"
    xmlns="http://www.w3.org/2000/svg"
    aria-labelledby="textsTitleID textsDescID"
  >
    <title id="textsTitleID">Texts icon</title>
    <desc id="textsDescID">An illustration of an open book.</desc>
    <path
      class="fill-color"
      d="m10.3323235 11.0007023h6.9060825c.8851083 0 1.5847122.3064258 2.0988114.9192774v14.4324451h-.6460032c-.1435563-.120323-.3528315-.2434552-.6278257-.3693964-.2749942-.1259413-.5201585-.2191097-.7354929-.2795053l-.3048241-.1081503h-5.7042647c-.3108832 0-.5621067-.0601615-.7536705-.1804846-.0717781-.0599274-.1256117-.1439663-.1615008-.2521166-.0358891-.1081502-.0598928-.2043619-.0720112-.2886348v-13.8741368zm19.1752505 0v13.603761c-.0717781.3361555-.2211606.5943584-.4481473.7746089-.0717781.0599274-.1733862.1079162-.304824.1439663-.1314379.0360501-.2451643.0601615-.3411793.0723343h-5.5965975c-.9568865.2640552-1.5068748.5164059-1.649965.757052h-.6634817v-14.4324451c.5140992-.6128516 1.2076439-.9192774 2.0806339-.9192774h6.92426zm1.3814961.6489017-.1796783 15.2976474c-.0955489 0-1.0342578.0119386-2.8161268.035816-1.7818691.0238773-3.3006293.0898911-4.5562806.1980414-1.2556514.1081503-1.9613144.2884008-2.1169891.5407514-.0955488.1924233-.5439291.273419-1.345141.2429871-.8012118-.0304319-1.3155441-.1776755-1.5429969-.4417308-.334654-.3843783-3.4558378-.5765674-9.36355164-.5765674v-15.3875385l-.96830576.3960828v16.2702977c6.4096947-.2041278 9.7760429-.0840388 10.0990445.3602669.2391051.276228.9864833.414342 2.2421347.414342.1915638 0 .4187835-.0210682.6816593-.0632047s.4810068-.0870821.6543929-.1348367c.1733862-.0477547.2719646-.0838048.2957353-.1081503.0838965-.1563732.9599161-.2675666 2.6280587-.3335805 1.6681426-.0660138 3.3213703-.0931684 4.9596831-.0814638l2.4392915.0182591v-16.2344816z"
    />
  </svg>
`,Xr=p`
  <svg
    width="40"
    height="40"
    viewBox="0 0 40 40"
    xmlns="http://www.w3.org/2000/svg"
    aria-labelledby="uploadTitleID uploadDescID"
  >
    <title id="uploadTitleID">Upload icon</title>
    <desc id="uploadDescID">
      An illustration of a horizontal line over an up pointing arrow.
    </desc>
    <path
      class="fill-color"
      d="m20 12.8 8 10.4h-4.8v8.8h-6.4v-8.8h-4.8zm12-4.8v3.2h-24v-3.2z"
      fill-rule="evenodd"
    />
  </svg>
`,Jr=p`
  <svg
    height="14"
    width="14"
    viewBox="8 8 24 24"
    xmlns="http://www.w3.org/2000/svg"
    aria-labelledby="uploadTitleID uploadDescID"
  >
    <title id="uploadTitleID">Upload files</title>
    <path
      class="fill-color"
      d="m20 12.8 8 10.4h-4.8v8.8h-6.4v-8.8h-4.8zm12-4.8v3.2h-24v-3.2z"
      fill-rule="evenodd"
    />
  </svg>
`,en=p`
  <svg
    width="40"
    height="40"
    viewBox="0 0 40 40"
    xmlns="http://www.w3.org/2000/svg"
    aria-labelledby="userTitleID userDescID"
  >
    <title id="userTitleID">User icon</title>
    <desc id="userDescID">An illustration of a person's head and chest.</desc>
    <path
      class="fill-color"
      d="m20.7130435 18.0434783c-3.5658385 0-6.4565218-2.9198821-6.4565218-6.5217392 0-3.60185703 2.8906833-6.5217391 6.4565218-6.5217391s6.4565217 2.91988207 6.4565217 6.5217391c0 3.6018571-2.8906832 6.5217392-6.4565217 6.5217392zm-12.9130435 16.9565217c0-7.9240855 5.7813665-14.3478261 12.9130435-14.3478261s12.9130435 6.4237406 12.9130435 14.3478261z"
      fill-rule="evenodd"
    />
  </svg>
`,tn=p`
  <svg
    height="40"
    viewBox="0 0 40 40"
    width="40"
    xmlns="http://www.w3.org/2000/svg"
    aria-labelledby="videoTitleID videoDescID"
  >
    <title id="videoTitleID">Video icon</title>
    <desc id="videoDescID">An illustration of two cells of a film strip.</desc>
    <path
      class="fill-color"
      d="m31.0117647 12.0677966c0 .4067797-.2823529.6779661-.7058823.6779661h-1.2705883c-.4235294 0-.7058823-.2711864-.7058823-.6779661v-.6779661c0-.4067797.2823529-.6779661.7058823-.6779661h1.2705883c.4235294 0 .7058823.2711864.7058823.6779661zm0 3.2542373c0 .4067797-.2823529.6779661-.7058823.6779661h-1.2705883c-.4235294 0-.7058823-.2711864-.7058823-.6779661v-.6779661c0-.4067797.2823529-.6779661.7058823-.6779661h1.2705883c.4235294 0 .7058823.2711864.7058823.6779661zm0 3.2542373c0 .4067796-.2823529.6779661-.7058823.6779661h-1.2705883c-.4235294 0-.7058823-.2711865-.7058823-.6779661v-.6779661c0-.4067797.2823529-.6779661.7058823-.6779661h1.2705883c.4235294 0 .7058823.2711864.7058823.6779661zm0 3.3898305c0 .4067797-.2823529.6779661-.7058823.6779661h-1.2705883c-.4235294 0-.7058823-.2711864-.7058823-.6779661v-.6779661c0-.4067797.2823529-.6779661.7058823-.6779661h1.2705883c.4235294 0 .7058823.2711864.7058823.6779661zm0 3.2542373c0 .4067796-.2823529.6779661-.7058823.6779661h-1.2705883c-.4235294 0-.7058823-.2711865-.7058823-.6779661v-.6779661c0-.4067797.2823529-.6779661.7058823-.6779661h1.2705883c.4235294 0 .7058823.2711864.7058823.6779661zm0 3.2542373c0 .4067796-.2823529.6779661-.7058823.6779661h-1.2705883c-.4235294 0-.7058823-.2711865-.7058823-.6779661v-.6779661c0-.4067797.2823529-.6779661.7058823-.6779661h1.2705883c.4235294 0 .7058823.2711864.7058823.6779661zm-4.0941176-10.440678c0 .5423729-.4235295.9491525-.9882353.9491525h-11.5764706c-.5647059 0-.9882353-.4067796-.9882353-.9491525v-6.9152542c0-.5423729.4235294-.9491526.9882353-.9491526h11.5764706c.5647058 0 .9882353.4067797.9882353.9491526zm-.1411765 11.2542373c0 .5423729-.4235294.9491525-.9882353.9491525h-11.5764706c-.5647059 0-.9882353-.4067796-.9882353-.9491525v-6.9152542c0-.5423729.4235294-.9491526.9882353-.9491526h11.5764706c.5647059 0 .9882353.4067797.9882353.9491526zm-14.9647059-17.220339c0 .4067797-.2823529.6779661-.7058823.6779661h-1.27058828c-.42352941 0-.70588236-.2711864-.70588236-.6779661v-.6779661c0-.4067797.28235295-.6779661.70588236-.6779661h1.27058828c.4235294 0 .7058823.2711864.7058823.6779661zm0 3.2542373c0 .4067797-.2823529.6779661-.7058823.6779661h-1.27058828c-.42352941 0-.70588236-.2711864-.70588236-.6779661v-.6779661c0-.4067797.28235295-.6779661.70588236-.6779661h1.27058828c.4235294 0 .7058823.2711864.7058823.6779661zm0 3.2542373c0 .4067796-.2823529.6779661-.7058823.6779661h-1.27058828c-.42352941 0-.70588236-.2711865-.70588236-.6779661v-.6779661c0-.4067797.28235295-.6779661.70588236-.6779661h1.27058828c.4235294 0 .7058823.2711864.7058823.6779661zm0 3.3898305c0 .4067797-.2823529.6779661-.7058823.6779661h-1.27058828c-.42352941 0-.70588236-.2711864-.70588236-.6779661v-.6779661c0-.4067797.28235295-.6779661.70588236-.6779661h1.27058828c.4235294 0 .7058823.2711864.7058823.6779661zm0 3.2542373c0 .4067796-.2823529.6779661-.7058823.6779661h-1.27058828c-.42352941 0-.70588236-.2711865-.70588236-.6779661v-.6779661c0-.4067797.28235295-.6779661.70588236-.6779661h1.27058828c.4235294 0 .7058823.2711864.7058823.6779661zm0 3.2542373c0 .4067796-.2823529.6779661-.7058823.6779661h-1.27058828c-.42352941 0-.70588236-.2711865-.70588236-.6779661v-.6779661c0-.4067797.28235295-.6779661.70588236-.6779661h1.27058828c.4235294 0 .7058823.2711864.7058823.6779661zm20.0470588-20.4745763h-.8470588v.27118644.6779661c0 .40677966-.2823529.6779661-.7058823.6779661h-1.2705883c-.4235294 0-.7058823-.27118644-.7058823-.6779661v-.6779661-.27118644h-16.5176471v.27118644.6779661c0 .40677966-.2823529.6779661-.7058823.6779661h-1.27058828c-.42352941 0-.70588236-.27118644-.70588236-.6779661v-.6779661-.27118644h-1.12941176v24h1.12941176v-.2711864-.6779661c0-.4067797.28235295-.6779661.70588236-.6779661h1.27058828c.4235294 0 .7058823.2711864.7058823.6779661v.6779661.2711864h16.6588235v-.2711864-.6779661c0-.4067797.282353-.6779661.7058824-.6779661h1.2705882c.4235294 0 .7058824.2711864.7058824.6779661v.6779661.2711864h.8470588v-24z"
      fill-rule="evenodd"
    />
  </svg>
`,rn=p`
  <svg
    height="40"
    viewBox="0 0 40 40"
    width="40"
    xmlns="http://www.w3.org/2000/svg"
    aria-labelledby="webTitleID webDescID"
  >
    <title id="webTitleID">Web icon</title>
    <desc id="webDescID">An illustration of a computer application window</desc>
    <path
      class="fill-color"
      d="m8 28.7585405v-8.1608108-9.3577297h24v9.3577297 8.1608108zm14.2702703-15.8863783h-12.43243246v2.6114594h12.43243246zm7.7837838 14.0365946v-7.0727027-1.8497838h-20.21621626v1.8497838 7.0727027zm-3.7837838-14.0365946h-2.7027027v2.6114594h2.7027027zm4 0h-2.7027027v2.6114594h2.7027027z"
      fill-rule="evenodd"
    />
  </svg>
`,O={audio:Hr,close:Fr,donate:Wr,donateUnpadded:jr,ellipses:qr,iaLogo:Kr,images:Gr,search:Yr,software:Zr,texts:Qr,upload:Xr,uploadUnpadded:Jr,user:en,video:tn,web:rn},L=(i="",e)=>/^https?:/.test(i)?i:`${e}${i}`;var bt;let Ue=bt=class extends B{constructor(){super(...arguments),this.baseHost="",this.menuItems=[]}static get styles(){return Vr}get listItems(){return this.menuItems?this.menuItems.map(e=>p`
            <li>
              <a
                class="${e.title.toLowerCase()}"
                .href="${L(e.url,this.baseHost)}"
                >${e.title}${bt.iconFor(e.title)}</a
              >
            </li>
          `):y}static iconFor(e){const t={Donate:O.donate};return t[e]?t[e]:p``}render(){return p`
      <ul>
        ${this.listItems}
      </ul>
    `}};c([h({type:String})],Ue.prototype,"baseHost",void 0);c([h({type:Array})],Ue.prototype,"menuItems",void 0);Ue=bt=c([A("desktop-subnav")],Ue);function wt(i){return i?"true":"false"}const It=g`
  .nav-container {
    position: relative;
  }

  nav {
    position: absolute;
    right: 0;
    z-index: 4;
    overflow: hidden;
    font-size: 1.6rem;
    background-color: var(--dropdownMenuBg);
    transition-property: top;
    transition-duration: 0.2s;
    transition-timing-function: ease;
  }

  .initial,
  .closed {
    top: var(--topOffset, -1500px);
  }

  .closed {
    transition-duration: 0.5s;
  }

  .open {
    max-width: 100vw;
    overflow: auto;
  }

  h3 {
    padding: 0.6rem 2rem;
    margin: 0;
    font-size: inherit;
    overflow: hidden;
    text-overflow: ellipsis;
  }

  ul {
    padding: 0.4rem 0 0.7rem 0;
    margin: 0;
    list-style: none;
    /* viewport height - nav height + bottom nav border */
    max-height: calc(100vh - 7.2rem + 1px);
    overflow: auto;
    box-sizing: border-box;
  }

  .divider {
    margin: 0.5rem 0;
    border-bottom: 1px solid var(--dropdownMenuDivider);
  }

  a,
  .info-item {
    display: block;
    color: var(--primaryTextColor);
    text-decoration: none;
    padding: 1rem 2rem;
  }

  .info-item {
    font-size: 0.8em;
    color: var(--dropdownMenuInfoItem);
  }

  .callout {
    position: absolute;
    margin-left: 10px;
    padding: 0 5px;
    border-radius: 2px;
    background: #fee257;
    color: #2c2c2c;
    font-size: 1.4rem;
    font-weight: bold;
  }

  a.mobile-upload {
    display: flex;
    justify-content: left;
    align-items: center;
  }
  a.mobile-upload svg {
    fill: var(--white);
    margin-right: 1rem;
    height: 1.4rem;
    width: 1.4rem;
  }

  @media (min-width: 890px) {
    nav {
      display: flex;
      overflow: visible;
      top: 0;
      left: auto;
      z-index: 5;
      transition: opacity 0.2s ease-in-out;
      font-size: 1.4rem;
      border-radius: 2px;
      background: var(--primaryTextColor);
      box-shadow: 0 1px 2px 1px rgba(0, 0, 0, 0.15);
    }

    nav:after {
      position: absolute;
      right: 7px;
      top: -7px;
      width: 12px;
      height: 7px;
      box-sizing: border-box;
      color: var(--white);
      content: '';
      border-bottom: 7px solid currentColor;
      border-left: 6px solid transparent;
      border-right: 6px solid transparent;
    }

    h3 {
      display: none;
    }

    ul {
      /* viewport height - nav height + bottom nav border */
      max-height: calc(100vh - 8.5rem + 1px);
    }

    .divider {
      border-bottom-color: var(--dropdownMenuDivider);
    }

    a {
      padding: 0.5rem 2rem;
      color: var(--inverseTextColor);
      transition:
        background 0.1s ease-out,
        color 0.1s ease-out;
    }

    .info-item {
      padding: 0.5rem 2rem;
      font-size: 0.8em;
      color: var(--inverseDropdownMenuInfoItem);
    }

    a:hover,
    a:active,
    a:focus {
      color: var(--linkHoverColor);
      background: var(--linkColor);
      outline: none;
    }

    .initial,
    .closed {
      opacity: 0;
      transition-duration: 0.2s;
    }

    .open {
      opacity: 1;
      overflow: visible;
    }

    a.mobile-upload {
      display: none;
    }
  }
`;/**
 * @license
 * Copyright 2018 Google LLC
 * SPDX-License-Identifier: BSD-3-Clause
 */const Ct=i=>i??k;class xt{constructor(e,t){var r;this.elementsContainer=e,this.menuOption=t,this.focusableElements=this.getFocusableElements(),this.focusedIndex=0,t!=="search"&&((r=this.focusableElements[this.focusedIndex])===null||r===void 0||r.focus()),this.handleKeyDown=this.handleKeyDown.bind(this)}getFocusableElements(){var e,t,r,n,s,o;const l="a[href], button, input, [tabindex]",a=u=>!u.hasAttribute("disabled")&&u.getAttribute("aria-hidden")!=="true"&&u.getAttribute("tabindex")!=="-1";let d;if(this.menuOption==="web"){const u=(e=this.elementsContainer.querySelector("wayback-slider"))===null||e===void 0?void 0:e.shadowRoot,m=u==null?void 0:u.querySelector("wayback-search"),v=Array.from((r=(t=m==null?void 0:m.shadowRoot)===null||t===void 0?void 0:t.querySelectorAll(l))!==null&&r!==void 0?r:[]),b=Array.from((n=u==null?void 0:u.querySelectorAll(l))!==null&&n!==void 0?n:[]),x=u==null?void 0:u.querySelector("save-page-form"),C=Array.from((o=(s=x==null?void 0:x.shadowRoot)===null||s===void 0?void 0:s.querySelectorAll(l))!==null&&o!==void 0?o:[]);d=[...v,...b,...C]}else d=this.elementsContainer.querySelectorAll(l);return Array.from(d??[]).filter(a)}handleKeyDown(e){const{key:t}=e;["ArrowDown","ArrowRight","ArrowUp","ArrowLeft"].includes(t)?(this.handleArrowKey(t),e.preventDefault()):t==="Tab"&&this.handleTabKey(e)}handleArrowKey(e){["ArrowDown","ArrowRight"].includes(e)?this.focusNext():this.focusPrevious()}handleTabKey(e){var t;const r=e.shiftKey;this.emitFocusToOtherMenuItems(r),(t=this.focusableElements[this.focusedIndex])===null||t===void 0||t.blur(),["search"].includes(this.menuOption)||e.preventDefault()}focusPrevious(){var e;this.focusableElements.length!==0&&(this.focusedIndex=(this.focusedIndex-1+this.focusableElements.length)%this.focusableElements.length,(e=this.focusableElements[this.focusedIndex])===null||e===void 0||e.focus())}focusNext(){var e;this.focusableElements.length!==0&&(this.focusedIndex=(this.focusedIndex+1)%this.focusableElements.length,(e=this.focusableElements[this.focusedIndex])===null||e===void 0||e.focus())}emitFocusToOtherMenuItems(e=!1){this.elementsContainer.dispatchEvent(new CustomEvent("focusToOtherMenuItem",{bubbles:!0,composed:!0,detail:{mediatype:this.menuOption,moveTo:e?"prev":"next"}}))}}class R extends B{constructor(){super(...arguments),this.baseHost="",this.config=T,this.hideSearch=!1,this.menuItems=[],this.animated=!1,this.open=!1}static get styles(){return[It]}updated(e){var t;if(e.has("open")&&this.open){const r=(t=this.shadowRoot)===null||t===void 0?void 0:t.querySelector(".nav-container");if(r){const n=new xt(r,"usermenu");this.addEventListener("keydown",n.handleKeyDown),this.previousKeydownListener&&this.removeEventListener("keydown",this.previousKeydownListener),this.previousKeydownListener=n.handleKeyDown}}}get dropdownItems(){if(!this.menuItems)return y;if(!Array.isArray(this.menuItems[0])){const e=this.menuItems;return this.dropdownSection(e)}return this.menuItems.map((e,t)=>{const r=t?R.dropdownDivider:p``;if(Array.isArray(e))return[r,...this.dropdownSection(e)]})}static get dropdownDivider(){return p`<li class="divider"></li>`}dropdownSection(e){return e.map(t=>p`
        <li>
          ${t.url?this.dropdownLink(t):R.dropdownText(t)}
        </li>
      `)}dropdownLink(e){var t,r,n;const s=(r=(t=this.config)===null||t===void 0?void 0:t.callouts)===null||r===void 0?void 0:r[e.title],o=e.class==="mobile-upload",l=this.open&&!o;return p`<a
      href="${L(e.url,this.baseHost)}"
      class=${Ct(e.class)}
      tabindex="${l?"":"-1"}"
      @click=${this.trackClick}
      data-event-click-tracking="${(n=this.config)===null||n===void 0?void 0:n.eventCategory}|Nav${e.analyticsEvent}"
      aria-label=${s?`New feature: ${e.title}`:y}
    >
      ${o?O.uploadUnpadded:y} ${e.title}
      ${s?p`<span class="callout" aria-hidden="true">${s}</span>`:y}
    </a>`}static dropdownText(e){return p`<span class="info-item">${e.title}</span>`}get menuClass(){const e=this.hideSearch?" search-hidden":"";return this.open?`open${e}`:this.animated?`closed${e}`:`initial${e}`}render(){return p`
      <div class="nav-container">
        <nav
          class="${this.menuClass}"
          aria-hidden="${wt(!this.open)}"
          aria-expanded="${wt(this.open)}"
        >
          <ul>
            ${this.dropdownItems}
          </ul>
        </nav>
      </div>
    `}}c([h({type:String})],R.prototype,"baseHost",void 0);c([h({type:Object})],R.prototype,"config",void 0);c([h({type:Boolean})],R.prototype,"hideSearch",void 0);c([h({type:Array})],R.prototype,"menuItems",void 0);c([h({type:Boolean})],R.prototype,"animated",void 0);c([h({type:Boolean})],R.prototype,"open",void 0);const oi=g`var(--white, #fff)`,nn=g`var(--ia-theme-link-color, #4b64ff)`,sn=g`var(--primaryDisableCTAFill, #767676)`,on=g`var(--secondaryCTABorder, #999)`,an=g`var(--primaryCTAFill, #194880)`,st=g`var(--primaryCTAFillRGB, 25, 72, 128)`,ln=g`var(--primaryCTABorder, #c5d1df)`,cn=g`var(--primaryErrorCTAFill, #d9534f)`,ot=g`var(--primaryErrorCTAFillRGB, 229, 28, 38)`,dn=g`var(--primaryErrorCTABorder, #d43f3a)`,hn=g`var(--secondaryCTAFill, #333)`,at=g`var(--secondaryCTAFillRGB, 51, 51, 51)`,un=g`var(--primaryCTABorder, #979797)`,pn=g`var(---primaryWarningFill, #ee8950)`,lt=g`var(--primaryWarningFillRGB, 238, 137, 80)`,mn=g`var(--primaryWarningBorder, #ec7939)`,Ys=g`
  .ia-button {
    min-height: 3rem;
    cursor: pointer;
    color: ${oi};
    line-height: normal;
    border-radius: 0.4rem;
    font-size: 1.4rem;
    font-family: 'Helvetica Neue', Helvetica, Arial, sans-serif;
    border: 1px solid transparent;
    white-space: nowrap;
    appearance: auto;
    box-sizing: border-box;
    display: flex;
    align-items: center;
    transition: all 0.1s ease 0s;
    vertical-align: middle;
    padding: 0 3rem;
    outline-color: ${oi};
    outline-offset: -4px;
    user-select: none;
    text-decoration: none;
    width: fit-content;
    -webkit-user-select: none;
    -moz-user-select: none;
    -ms-user-select: none;
    -o-user-select: none;
  }
  .ia-button:focus-visible {
    outline-style: double;
  }
  .ia-button:disabled {
    cursor: not-allowed;
    background-color: ${sn};
    border: 1px solid ${on};
  }
  .ia-button.transparent {
    background-color: transparent;
  }

  .ia-button.primary:disabled,
  .ia-button.danger:disabled,
  .ia-button.warning:disabled,
  .ia-button.dark:disabled {
    opacity: 0.5;
  }

  .ia-button.primary {
    background-color: ${an};
    border-color: ${ln};
  }
  .ia-button.primary:hover {
    background-color: rgba(${st}, 0.9);
  }
  .ia-button.primary:focus-visible {
    background-color: rgba(${st}, 0.8);
  }
  .ia-button.primary:active {
    background-color: rgba(${st}, 0.7);
  }

  .ia-button.danger {
    background-color: ${cn};
    border-color: ${dn};
  }
  .ia-button.danger:hover {
    background-color: rgba(${ot}, 0.9);
  }
  .ia-button.danger:focus-visible {
    background-color: rgba(${ot}, 0.8);
  }
  .ia-button.danger:active {
    background-color: rgba(${ot}, 0.7);
  }

  .ia-button.warning {
    background-color: ${pn};
    border-color: ${mn};
  }
  .ia-button.warning:hover {
    background-color: rgba(${lt}, 0.9);
  }
  .ia-button.warning:focus-visible {
    background-color: rgba(${lt}, 0.8);
  }
  .ia-button.warning:active {
    background-color: rgba(${lt}, 0.7);
  }

  .ia-button.dark {
    background-color: ${hn};
    border-color: ${un};
  }
  .ia-button.dark:hover {
    background-color: rgba(${at}, 0.9);
  }
  .ia-button.dark:focus-visible {
    background-color: rgba(${at}, 0.8);
  }
  .ia-button.dark:active {
    background-color: rgba(${at}, 0.7);
  }

  .ia-button.link {
    margin: 0;
    padding: 6px;
    border: 0;
    appearance: none;
    background: none;
    color: ${nn};
    text-decoration: none;
    cursor: pointer;
  }
  .ia-button.link:hover {
    text-decoration: underline;
  }
`,Lt=g`
  .sr-only {
    position: absolute !important;
    width: 1px !important;
    height: 1px !important;
    margin: -1px !important;
    padding: 0 !important;
    border: 0 !important;
    overflow: hidden !important;
    white-space: nowrap !important;
    clip: rect(1px, 1px, 1px, 1px) !important;
    -webkit-clip-path: inset(50%) !important;
    clip-path: inset(50%) !important;
    user-select: none !important;
  }
`,vn=p`
  <svg
    height="40"
    viewBox="0 0 40 40"
    width="40"
    xmlns="http://www.w3.org/2000/svg"
    aria-labelledby="searchTitleID searchDescID"
  >
    <title id="searchTitleID">Search icon</title>
    <desc id="searchDescID">An illustration of a magnifying glass.</desc>
    <path
      class="fill-color"
      d="m32.4526364 29.8875889-8.1719472-7.9751279c1.1046135-1.4876138 1.7652549-3.3102407 1.7652549-5.2846451 0-.101185-.0142895-.1981539-.030573-.2944743.0166158-.0976175.0309053-.196208.0309053-.2990145 0-4.9814145-4.152935-9.0343271-9.2572866-9.0343271-.0907218 0-.1781206.01394537-.2655193.02594487-.0880633-.0119995-.1747974-.02594487-.2655193-.02594487-5.1046839 0-9.25761889 4.0529126-9.25761889 9.0343271 0 .1011849.01395722.1981539.03057294.2947985-.01694804.0976176-.03090525.1958838-.03090525.2986903 0 4.9814145 4.1526027 9.0346514 9.2572866 9.0346514.0907218 0 .1777882-.0139454.2658516-.0262692.0873987.0123238.1741328.0262692.265187.0262692 1.7306942 0 3.3467399-.4747911 4.7338208-1.2852439l8.2882574 8.0886366c.3652137.3564177.843082.53414 1.3212826.53414.4782007 0 .9567336-.1780467 1.3212827-.53414.7294304-.7118622.7294304-1.8660845-.0003323-2.5782711zm-15.9526364-7.8875889c-.0832667-.0118703-.1652765-.0253024-.2513711-.0253024-2.8781993 0-5.2197212-2.3278242-5.2197212-5.1891862 0-.0974612-.013197-.1908615-.0289077-.2836371.0160249-.0940251.0292219-.1889874.0292219-.2880105 0-2.861362 2.3418361-5.1891861 5.2200354-5.1891861.0854662 0 .1677902-.0131198.2510569-.0246777.0826383.0115579.1649623.0246777.2510569.0246777 2.8781993 0 5.2197212 2.3278241 5.2197212 5.1891861 0 .0974612.0135112.1908616.0289077.2839496-.0157107.0940251-.0295361.1886749-.0295361.287698 0 2.861362-2.3415219 5.1891862-5.2197212 5.1891862-.0860946 0-.1684187.0134321-.2507427.0253024z"
      fill-rule="evenodd"
    />
  </svg>
`,gn=p`
  <svg
    height="55"
    viewBox="0 0 205 55"
    width="205"
    xmlns="http://www.w3.org/2000/svg"
  >
    <g fill="none">
      <path
        d="m41.5442526 5.47625158v36.05869762s-.0825635.8039669.6808836.8039669c.7634442 0 .5779563-.8039669.5779563-.8039669v-36.05869762h5.4674088l.0001982 44.76728562c.0070059.1547261.1602174 4.7191467-6.1484876 4.7191467-6.1999371 0-6.1018528-4.697561-6.0880787-5.0160398l.0008628-1.7899844h5.5092566l.0006247 1.8215446c.0064131.111431.0698016.5292973.6598981.5292973.6808836 0 .5564693-.5981684.5564693-.5981684v-4.3726361s-.9489378 1.6294184-3.4044061 1.2370455c-2.4554712-.39237-2.8060919-1.8148639-2.9712219-2.4967105-.1651328-.6795829-.2680542-1.174855-.2680542-2.0625 0-.8599178-.0383122-34.61864321-.0406109-36.64339281l-.0001077-.09488771zm30.6351382 1.09344342c6.9117365 0 6.7805382 5.4445918 6.7805382 5.4445918v39.5210721h-5.7365952v-1.1522413s-2.086754 2.4978424-5.1507198.8955592c-3.0650979-1.6011513-2.5436924-5.1879097-2.5436924-5.1879097l.0000351-8.8028589c.0025794-.2398669.0684122-2.0251881 1.4352444-3.3674745 1.4669498-1.4405832 3.4553051-1.2178245 3.4553051-1.2178245h2.5425603v-20.4327721s-.0656021-.8966882-.7170766-.8966882c-.6526037 0-.6854033.9566179-.6854033.9566179v16.080489h-6.030665l-.0005626-16.2852138c-.0087587-.3366877-.0366527-5.555347 6.6510318-5.555347zm28.5031432-6.569695v31.41468l.711418.0192211c.593792 0 .593792-.4941403.593792-.4941403v-24.86759869h5.897202v21.90049339s.079171 1.404399-.791723 2.512539c-.870895 1.1081429-1.820962 1.1477197-1.820962 1.1477197s.989654.3946337 1.860548 1.4439758c.870894 1.0482132.791724 2.4921889.791724 2.4921889v14.4024477h-5.936789l-.000309-13.3243353c-.002165-.043998-.019484-.2297601-.158037-.3860879-.158343-.1775284-.277102-.1775284-.277102-.1775284h-.890123v13.8879516h-5.8180296v-49.9715266zm-14.2340414 8.02384579c5.9675612 0 6.2060902 4.59291681 6.2148001 5.06804611l.0002664 15.2041578h-5.556805v-15.4269123s-.0531603-.5009227-.6062332-.5009227c-.5519582 0-.5790885.42064-.5790885.42064v32.2254321s.079173.4477782.6582614.4477782c.4900102 0 .5467595-.3205986.552623-.4192443l.00045-8.2163433h5.4775891l.000339 7.8754281c.0126595.1654646.3391417 5.1294029-6.0570166 5.1294029-6.504565 0-6.1358477-5.1867779-6.1358477-5.1867779l-.0006428-31.5032204c-.0096883-.3249184-.0495263-5.11746431 6.0313048-5.11746431zm-73.7580006-5.70016816v42.49270187h.5575988s.2081099.2374581.5010473-.0554082.1956685-.3901121.1956685-.3901121v-42.04605262l5.7004019.00115789.0007682 41.01721663c.015678.1470376.248026 2.5994908-1.4903372 4.3374305-1.792686 1.7922473-4.4053704 1.6101973-4.4053704 1.6101973h-7.83579091s-2.61381542.18205-4.40650134-1.6101973c-1.79268592-1.7922503-1.48843833-4.344369-1.48843833-4.344369v-41.01027813l5.69927118-.00115789v42.04605262s-.09726862.0972458.19566849.3901121c.29293741.2928663.50217829.0554082.50217829.0554082h.55646783v-42.49270187zm44.8442957-.59701342s2.8128759.11081342 4.2515488 1.54913579 1.3572386 4.52302632 1.3572386 4.52302632v20.17269738s.1085791 1.6825658-.8324397 2.4967105c-.9410187.8141447-1.5743967.9950658-1.5743967.9950658s1.0495978.5789474 1.6105898 1.3026316c.5609919.7236842.7419571 1.3930921.7419571 2.3881579l.0007115 7.6426204c.0126941.1435677.1783816 2.2493941-.8874408 3.6468533-1.1038873 1.4473684-2.4430294 2.1348684-5.2298927 2.1348684h-6.4604558v-46.85176739zm14.5337626 35.74095399s-.1673942-.0203558-.3777654.1843136c-.2092421.2069274-.1459043.3301795-.1459043.3301795v8.4524058s.1673942.4523026.7329089.4523026c.5643854 0 .648084-.4523026.648084-.4523026v-8.9465489zm-44.3104484-34.72101373c5.9643432 0 6.16707 4.5349604 6.1720848 5.00409423l-.0000412 36.6461958h-5.2231058v-1.0674342s-1.8990049 2.3135258-4.6881295.8299731c-2.7891246-1.4824208-2.3140925-4.8057147-2.3140925-4.8057147l.0000416-8.1558683c.0025667-.229176.064831-1.8776574 1.3051717-3.1166422 1.3357486-1.3354261 3.1454001-1.1273669 3.1454001-1.1273669h2.3140896v-18.92660288s-.0588122-.831105-.6526037-.831105-.6232005.88538132-.6232005.88538132v14.89545606h-5.4888988l-.0005553-15.08585244c-.0080458-.3266358-.0237133-5.14451389 6.0538395-5.14451389zm28.932117 33.13115253-.4776089-.0000064v8.3947599h.705764c.5067025 0 .560992-.7236842.560992-.7236842v-6.712171c0-.9769737-.7962466-.9589047-.7962466-.9589047zm-29.0339092-4.5105709s-.1526883-.0203529-.3438338.1707431c-.1900134.191099-.1323304.305305-.1323304.305305v7.8282282s.1515591.419511.6661776.419511c.5146186 0 .5915302-.419511.5915302-.419511v-8.2861842zm29.0087131-25.54039926c-.6808813 0-.5443092.01385177-.4995274.01731471l.0109215.0007774v25.52796055s1.3391421.1085526 1.3391421-1.3569079v-23.35690792s.1085791-.83223684-.8505362-.83223684z"
        fill="#ab2e33"
      />
      <path
        d="m164.347608 2.25946661v7.4442539c.21729-.35229187 1.198489-1.67028746 3.735798-1.67028746 3.534354 0 3.281982 3.91684325 3.281982 3.91684325v39.9180342h-3.988173l-.000094-38.4191634c-.002249-.1296195-.058484-1.7498629-1.464346-1.7498629-1.087582 0-1.450864.5267465-1.565167 1.1683005v39.1020256h-4.190748v-49.71014369zm23.367699 5.4959617c3.58528 0 3.32838 3.94160559 3.32838 3.94160559v40.1712766h-4.045888l-.000761-38.6884533c-.008742-.2394578-.118025-1.7360686-1.484053-1.7360686-1.103423 0-1.471231.5301234-1.587799 1.1750516v39.3496435h-4.250729v-43.92154285h4.250729v1.38890288c.220685-.35566881 1.216596-1.68041542 3.790121-1.68041542zm11.392998-.52112204c6.124855 0 5.736674 5.39691513 5.736674 5.39691513v24.2844279h-7.17056l.000077 9.2728055c.001997 1.1852594.055922 2.3291557 1.511897 2.3291557 1.511974 0 1.588933-1.3483852 1.588933-1.3483852v-6.1679026h4.108134l-.00035 5.6892637c-.01289 1.7887967-.410235 4.8333128-5.736327 4.8333128-5.22451 0-5.490421-3.8596058-5.502952-4.1455025l-.000589-34.6706379s-.65866-5.47345253 5.465063-5.47345253zm-72.839588-6.23430627c4.55044 0 4.748706 4.49835393 4.756437 5.01739476l.000233 44.68489444h-4.246265l-.000015-45.39546442c-.001065-.11122824-.038841-1.27912478-1.155468-1.27912478-.689016 0-1.239407-.0003988-1.575847-.00072723l-.366181-.00042531v46.67574174h-4.288077v-46.69262361h-1.942025c-1.0608 0-1.147931 1.05402663-1.154906 1.2547936l-.00058 45.43783001h-4.296023l.000269-45.03840883c.008379-.46638804.223774-4.66388037 5.263374-4.66388037zm12.746531 4.73059864c5.939252 0 5.529572 4.80151166 5.529572 4.80151166v39.1864407h-3.791254v-1.7040541s-.748065 1.776088-2.920962 1.8481219-4.418224-.1913422-4.418224-3.9371049l.000537-10.0356421c.009666-.3007246.1911-3.4583326 3.548528-3.4583326h3.5253l-.000596-22.2422593c-.009853-.1121552-.157986-1.45801702-1.592862-1.45801702-1.497271 0-1.484813 1.94491522-1.484813 1.94491522v17.4051907h-3.99949l-.000952-17.7112484c-.014831-.304231-.125641-4.63952176 5.605216-4.63952176zm12.457944.74622797c5.704988 0 5.452616 4.56852529 5.452616 4.56852529v16.2188817h-3.836522v-16.7703934s-.202578-1.40578478-1.51537-1.40578478c-1.193447 0-1.427274 1.16180558-1.459925 1.37304298l-.004518.0327418v32.5368129c0 .9038006.353096 1.9584201 1.565167 1.9584201 1.212068 0 1.363719-1.3551363 1.363719-1.3551363v-9.239474h3.786725l.000848 7.3579585c.013087 2.5821014.10472 5.9480539-5.301813 5.9480539-5.604264 0-5.201371-4.7699955-5.201371-4.7699955l-.000475-31.3866454c-.017056-.171763-.453979-5.06700779 5.150919-5.06700779zm26.215101 3.66584829v37.0051649h-3.533221v-37.0051649zm-37.199548 25.1702202h-1.714552s-.530775.0720339-.917823.4558391c-.385924.3838082-.409681.9848389-.409681.9848389v8.5236357s.096195 1.56111 1.568559 1.56111c1.473497 0 1.473497-1.6095052 1.473497-1.6095052zm58.697648-25.2264959c-1.433886 0-1.511974 1.7344408-1.511974 1.7344408v21.9725922h3.100907v-22.1256642s-.155047-1.5813688-1.588933-1.5813688zm-23.264712-5.31700073c1.129455 0 2.045015.68657238 2.045015 1.53184407 0 .84639831-.91556 1.53184695-2.045015 1.53184695s-2.045012-.68544864-2.045012-1.53184695c0-.84527169.915557-1.53184407 2.045012-1.53184407z"
        fill="#211e1e"
      />
    </g>
  </svg>
`;let ee=class extends P{constructor(){super(...arguments),this.queryHandler={performQuery:e=>window.location.href=`https://web.archive.org/web/*/${e}`},this.waybackPagesArchived="916 billion"}render(){return p`
      <form method="post" @submit=${this.handleSubmit}>
        <p>
          Search the history of more than ${this.waybackPagesArchived}
          <a
            @click=${this.emitWaybackMachineStatsLinkClicked}
            data-event-click-tracking="TopNav|WaybackMachineStatsLink"
            href="https://blog.archive.org/2016/10/23/defining-web-pages-web-sites-and-web-captures/"
            >web pages</a
          >
          on the Internet.
        </p>
        <fieldset>
          <a
            @click=${this.emitWaybackMachineLogoLinkClicked}
            data-event-click-tracking="TopNav|WaybackMachineLogoLink"
            aria-label="Visit the Wayback Machine"
            href="https://web.archive.org"
            >${gn}</a
          >
          <div class="search-field">
            <label for="url" class="sr-only">Search the Wayback Machine</label>
            <input
              type="text"
              name="url"
              id="url"
              placeholder="Enter URL or keywords"
            />
            ${vn}
          </div>
        </fieldset>
      </form>
    `}handleSubmit(e){e.preventDefault();const t=this.urlInput.value;this.emitWaybackSearchSubmitted(t),this.queryHandler.performQuery(t)}emitWaybackSearchSubmitted(e){this.dispatchEvent(new CustomEvent("waybackSearchSubmitted",{detail:{query:e}}))}emitWaybackMachineStatsLinkClicked(){this.dispatchEvent(new CustomEvent("waybackMachineStatsLinkClicked"))}emitWaybackMachineLogoLinkClicked(){this.dispatchEvent(new CustomEvent("waybackMachineLogoLink"))}};ee.styles=[Lt,g`
      :host {
        font: normal 1.2rem/1.5 var(--themeFontFamily);
      }

      form {
        max-width: 600px;
      }

      p {
        margin-top: 0;
        font-weight: 200;
      }

      a {
        font-weight: 500;
        text-decoration: none;
        color: var(--activeColor);
      }

      fieldset {
        padding: 0.7rem 2rem;
        margin: 1.5rem 0;
        box-sizing: border-box;
        text-align: center;
        border: none;
        border-radius: 7px;
        background-color: #fcf5e6;
        box-shadow: 3px 3px 0 0 #c3ad97;
      }

      fieldset a {
        font-size: 0;
      }

      img {
        width: 100%;
        max-width: 215px;
        max-height: 60px;
        margin-bottom: 1.3rem;
        vertical-align: middle;
      }

      input {
        display: block;
        width: 100%;
        height: 3rem;
        padding: 0.5rem 1rem 0.5rem 3rem;
        font: normal 1.2rem/1.5 var(--themeFontFamily);
        color: var(--ia-theme-secondary-text-color, #666);
        box-sizing: border-box;
        border: 1px solid var(--grey80);
        border-radius: 2rem;
        background: var(--ia-theme-secondary-background-color, #fff);
      }

      input:focus {
        border-color: #66afe9;
        outline: none;
      }

      .search-field {
        position: relative;
        overflow: hidden;
      }

      .search-field svg {
        position: absolute;
        top: 3px;
        left: 3px;
        width: 2.4rem;
        height: 2.4rem;
      }

      .search-field .fill-color {
        fill: var(--iconFill);
      }

      @media (min-width: 890px) {
        form {
          margin: 0 auto;
        }

        p {
          margin-bottom: 3rem;
          font-size: 1.6rem;
          text-align: center;
        }

        img {
          margin: 0;
        }

        fieldset {
          margin: 0 auto;
        }

        fieldset a,
        .search-field {
          display: inline-block;
          width: 49%;
          vertical-align: middle;
        }

        fieldset a {
          text-align: center;
        }

        .search-field svg {
          top: 2px;
        }

        .search-field .fill-color {
          fill: var(--desktopSearchIconFill);
        }
      }
    `];c([h({type:Object})],ee.prototype,"queryHandler",void 0);c([h({type:String})],ee.prototype,"waybackPagesArchived",void 0);c([re("#url")],ee.prototype,"urlInput",void 0);ee=c([A("ia-wayback-search")],ee);const ai=ee,fn=g`
  form {
  }

  p {
    margin-bottom: 1rem;
    font-size: 1.6rem;
    text-align: center;
  }

  fieldset {
    padding: 0.5rem;
    border-radius: 5px;
    box-shadow: none;
  }

  input {
    padding-left: 3rem;
    margin-top: 0.3rem;
    font-size: 1.4rem;
    border-color: #bca38e;
    background: #fff;
  }

  input::placeholder,
  input::-webkit-input-placeholder {
    color: #8e8e8e;
  }

  .search-field svg {
    top: 50%;
    transform: translateY(-50%);
  }

  @media (min-width: 890px) {
    fieldset a,
    .search-field {
      display: block;
      width: auto;
    }

    fieldset a {
      margin: 0 1.5rem;
    }
  }
`;let li=class extends ai{static get styles(){return[ai.styles,fn]}};li=c([A("wayback-search")],li);const yn=g`
  div {
    display: grid;
    grid-template-columns: 1fr auto;
    grid-column-gap: 0.8rem;
    margin: 0;
    padding: 0;
    border: none;
  }

  input[type='text'] {
    width: 100%;
    height: 3rem;
    box-sizing: border-box;
    border: 1px solid var(--savePageInputBorder);
    border-radius: 0.5rem;
    color: var(--grey13);
    font-size: inherit;
  }

  input[type='submit'] {
    -webkit-appearance: none;
    -moz-appearance: none;
    appearance: none;
    padding: 0.4rem 0.8rem;
    font: normal 1.3rem var(--themeFontFamily);
    text-transform: uppercase;
    color: var(--savePageSubmitText);
    border: none;
    border-radius: 16px;
    background: var(--savePageSubmitBg);
    cursor: pointer;
  }

  .error {
    display: none;
    margin-top: 0.5rem;
    font-weight: bold;
    color: var(--savePageErrorText);
  }

  .visible {
    display: block;
  }

  @media (min-width: 890px) {
    h3 {
      margin-top: 0;
      font: normal 100 1.6rem var(--themeFontFamily);
    }
  }
`;let Ne=class extends B{constructor(){super(...arguments),this.config=T,this.inputValid=!0}static get styles(){return[yn,Lt]}validateURL(e){const r=e.target.querySelector('[name="url_preload"]');if(!/\..{2,}$/.test(r.value)){e.preventDefault(),this.inputValid=!1;return}this.inputValid=!0,this.trackSubmit(e)}get errorClass(){return`error${this.inputValid?"":" visible"}`}render(){return p`
      <form
        action="//web.archive.org/save"
        method="post"
        data-event-submit-tracking="${this.config.eventCategory}|SavePageSubmit"
        @submit=${this.validateURL}
      >
        <h3>Save Page Now</h3>
        <p>
          Capture a web page as it appears now for use as a trusted citation in
          the future.
        </p>
        <div>
          <label for="url_preload" class="sr-only">Enter a URL to save</label>
          <input
            type="text"
            name="url_preload"
            id="url_preload"
            placeholder="https://"
          />
          <input type="submit" value="Save" />
        </div>
        <p class=${this.errorClass}>Please enter a valid web address</p>
      </form>
    `}};c([h({type:Object})],Ne.prototype,"config",void 0);c([S()],Ne.prototype,"inputValid",void 0);Ne=c([A("save-page-form")],Ne);const bn={performQuery(i){window.location.href=`https://web.archive.org/web/*/${i}`}},Mi=g`
  h4 {
    font-size: 1.6rem;
  }

  a {
    text-decoration: none;
    color: var(--activeColor);
  }

  ul {
    padding: 0;
    margin: 0;
    list-style: none;
  }

  li + li {
    padding-top: 1.5rem;
  }

  @media (min-width: 890px) {
    h4 {
      margin: 0 0 1rem 0;
      font-weight: 100;
    }

    ul {
      font-size: 1.3rem;
    }

    li {
      padding-bottom: 0.5rem;
    }

    li + li {
      padding-top: 0;
    }

    li a {
      display: block;
      overflow: hidden;
      white-space: nowrap;
      text-overflow: ellipsis;
    }
  }
`,wn=[Mi,g`
    @media (min-width: 890px) {
      :host {
        display: block;
        grid-column: 1 / 4;
        padding: 0 1.5rem;
      }

      h4 {
        margin-top: 0;
        font: normal 100 1.6rem var(--themeFontFamily);
      }

      .grid {
        display: grid;
        grid-template-columns: minmax(auto, 260px) 1fr minmax(auto, 260px);
        /* Possible for 890 - 935: minmax(auto, 260px) 1fr minmax(auto, 260px) */
        grid-column-gap: 2.5rem;
      }

      .link-lists {
        display: grid;
        grid-template-columns: calc(50% - 1.25rem) calc(50% - 1.25rem);
        grid-column-gap: 2.5rem;
      }
    }
  `],he=i=>{const e=i.split(" "),t=e.pop(),r=`${t==null?void 0:t.substr(0,1).toUpperCase()}${t==null?void 0:t.substr(1)}`;return e.length?he(`${e.join(" ")}${r}`):r};let te=class extends B{constructor(){super(...arguments),this.archiveItLinks=[],this.baseHost="",this.browserExtensionsLinks=[],this.config=T,this.mobileAppsLinks=[]}static get styles(){return wn}get mobileAppsItems(){return this.linkList(this.mobileAppsLinks,"Wayback")}get browserExtensionsItems(){return this.linkList(this.browserExtensionsLinks,"Wayback")}get archiveItItems(){return this.linkList(this.archiveItLinks,"ArchiveIt")}linkList(e,t){return e.map(r=>p`<li>
          <a
            .href=${L(r.url,this.baseHost)}
            @click=${this.trackClick}
            data-event-click-tracking="${this.analyticsEvent(`${t}${r.title}`)}"
            target=${r.external?"_blank":""}
            rel=${r.external?"noreferrer noopener":""}
            >${r.title}</a
          >
        </li>`)}analyticsEvent(e){var t;return`${(t=this.config)===null||t===void 0?void 0:t.eventCategory}|${he(e)}`}render(){var e;return p`
      <div class="grid">
        <wayback-search
          .waybackPagesArchived=${(e=this.config.waybackPagesArchived)!==null&&e!==void 0?e:""}
          .queryHandler=${bn}
        ></wayback-search>
        <div class="link-lists">
          <div>
            <h4>Mobile Apps</h4>
            <ul class="mobile-apps">
              ${this.mobileAppsItems}
            </ul>
            <h4>Browser Extensions</h4>
            <ul class="browser-extensions">
              ${this.browserExtensionsItems}
            </ul>
          </div>
          <div>
            <h4>Archive-It Subscription</h4>
            <ul class="archive-it">
              ${this.archiveItItems}
            </ul>
          </div>
        </div>
        <save-page-form .config=${this.config}></save-page-form>
      </div>
    `}};c([h({type:Array})],te.prototype,"archiveItLinks",void 0);c([h({type:String})],te.prototype,"baseHost",void 0);c([h({type:Array})],te.prototype,"browserExtensionsLinks",void 0);c([h({type:Object})],te.prototype,"config",void 0);c([h({type:Array})],te.prototype,"mobileAppsLinks",void 0);te=c([A("wayback-slider")],te);const $n=g`
  ul {
    padding: 0;
    margin: -1rem 0 0 0;
    list-style: none;
  }
  a {
    display: block;
    padding: 1rem 0;
    text-decoration: none;
    color: var(--activeColor);
  }
`;let Ee=class extends B{constructor(){super(...arguments),this.baseHost="",this.config=T,this.menuItems=[]}static get styles(){return $n}analyticsEvent(e){return`${this.config.eventCategory}|NavMore${he(e)}`}render(){return p`
      <ul>
        ${this.menuItems.map(e=>p`<li>
              <a
                @click=${this.trackClick}
                href=${L(e.url,this.baseHost)}
                data-event-click-tracking="${this.analyticsEvent(e.title)}"
                >${e.title}</a
              >
            </li>`)}
      </ul>
    `}};c([h({type:String})],Ee.prototype,"baseHost",void 0);c([h({type:Object})],Ee.prototype,"config",void 0);c([h({type:Array})],Ee.prototype,"menuItems",void 0);Ee=c([A("more-slider")],Ee);const _n=[Mi,g`
    img {
      display: block;
      width: 90px;
      height: 90px;
      margin: 0 auto 1rem auto;
      border-radius: 45px;
    }

    h3 {
      margin-top: 0;
      font-size: 1.8rem;
    }

    .icon-links {
      display: -webkit-box;
      display: -ms-flexbox;
      display: flex;
      -webkit-box-pack: space-evenly;
      -ms-flex-pack: space-evenly;
      justify-content: space-evenly;
      text-align: center;
    }

    .icon-links a {
      display: inline-block;
      width: 12rem;
      margin-bottom: 1.5rem;
      overflow: hidden;
      white-space: nowrap;
      text-align: center;
      text-overflow: ellipsis;
    }

    .icon-links a + a {
      margin-left: 2rem;
    }

    .featured h4 {
      display: none;
    }

    @media (min-width: 890px) {
      :host {
        display: -ms-grid;
        display: grid;
        -ms-grid-columns: 40% 20% 40%;
        grid-template-columns: 40% 20% 40%;
      }

      .wayback-search {
        -ms-grid-column: 1;
        -ms-grid-column-span: 3;
        grid-column: 1 / 4;
      }

      h3 {
        display: none;
      }

      .icon-links {
        -ms-grid-column: 1;
      }

      .icon-links a {
        padding-top: 3.5rem;
        max-width: 16rem;
      }

      .links {
        padding: 0 1.5rem;
      }

      .featured {
        -ms-grid-column: 2;
      }

      .featured h4 {
        display: block;
      }

      .top {
        -ms-grid-column: 3;
      }

      .top ul {
        display: -ms-grid;
        display: grid;
        -ms-grid-columns: 50% 3rem 50%;
        grid-template-columns: 50% 50%;
        -ms-grid-rows: (auto) [7];
        grid-template-rows: repeat(7, auto);
        grid-column-gap: 3rem;
        grid-auto-flow: column;
      }
      .top ul > *:nth-child(1) {
        -ms-grid-row: 1;
        -ms-grid-column: 1;
      }
      .top ul > *:nth-child(2) {
        -ms-grid-row: 2;
        -ms-grid-column: 1;
      }
      .top ul > *:nth-child(3) {
        -ms-grid-row: 3;
        -ms-grid-column: 1;
      }
      .top ul > *:nth-child(4) {
        -ms-grid-row: 4;
        -ms-grid-column: 1;
      }
      .top ul > *:nth-child(5) {
        -ms-grid-row: 5;
        -ms-grid-column: 1;
      }
      .top ul > *:nth-child(6) {
        -ms-grid-row: 6;
        -ms-grid-column: 1;
      }
      .top ul > *:nth-child(7) {
        -ms-grid-row: 7;
        -ms-grid-column: 1;
      }
      .top ul > *:nth-child(8) {
        -ms-grid-row: 1;
        -ms-grid-column: 3;
      }
      .top ul > *:nth-child(9) {
        -ms-grid-row: 2;
        -ms-grid-column: 3;
      }
      .top ul > *:nth-child(10) {
        -ms-grid-row: 3;
        -ms-grid-column: 3;
      }
      .top ul > *:nth-child(11) {
        -ms-grid-row: 4;
        -ms-grid-column: 3;
      }
      .top ul > *:nth-child(12) {
        -ms-grid-row: 5;
        -ms-grid-column: 3;
      }
      .top ul > *:nth-child(13) {
        -ms-grid-row: 6;
        -ms-grid-column: 3;
      }
      .top ul > *:nth-child(14) {
        -ms-grid-row: 7;
        -ms-grid-column: 3;
      }
    }
  `];var Le;let ue=Le=class extends B{constructor(){super(...arguments),this.baseHost="",this.config=T,this.menu="",this.menuItems=Le.defaultLinks,this.links=Le.defaultLinks}static get styles(){return _n}shouldUpdate(){return this.menuItems&&(this.links=this.menuItems),!0}static get defaultLinks(){return{heading:"",iconLinks:[],featuredLinks:[],links:[],mobileAppsLinks:[],browserExtensionsLinks:[],archiveItLinks:[]}}analyticsEvent(e){var t;return`${(t=this.config)===null||t===void 0?void 0:t.eventCategory}|${he(e)}${he(this.menu)}`}get iconLinks(){return this.links.iconLinks.map(e=>p`
        <a
          .href="${L(e.url,this.baseHost)}"
          @click=${this.trackClick}
          data-event-click-tracking="${this.analyticsEvent(e.title)}"
          ><img src="${Ct(e.icon)}" loading="lazy" />${e.title}</a
        >
      `)}renderLinks(e){return e.map(t=>p`
        <li>
          <a
            .href="${L(t.url,this.baseHost)}"
            @click=${this.trackClick}
            data-event-click-tracking="${this.analyticsEvent(t.title)}"
            >${t.title}</a
          >
        </li>
      `)}render(){return this.menu?(this.menuItems&&(this.links=this.menuItems),this.menu==="web"?p` <wayback-slider
        .baseHost=${this.baseHost}
        .config=${this.config}
        .archiveItLinks=${this.menuItems.archiveItLinks}
        .browserExtensionsLinks=${this.menuItems.browserExtensionsLinks}
        .mobileAppsLinks=${this.menuItems.mobileAppsLinks}
      ></wayback-slider>`:this.menu==="more"?p` <more-slider
        .baseHost=${this.baseHost}
        .config=${this.config}
        .menuItems=${this.menuItems.links}
      >
      </more-slider>`:p`
      <h3>${this.links.heading}</h3>
      <div class="icon-links">${this.iconLinks}</div>
      <div class="links featured">
        <h4>Featured</h4>
        <ul>
          ${this.renderLinks(this.links.featuredLinks)}
        </ul>
      </div>
      <div class="links top">
        <h4>Top</h4>
        <ul>
          ${this.renderLinks(this.links.links)}
        </ul>
      </div>
    `):p``}};c([h({type:String})],ue.prototype,"baseHost",void 0);c([h({type:Object})],ue.prototype,"config",void 0);c([h({type:String})],ue.prototype,"menu",void 0);c([h({type:Object})],ue.prototype,"menuItems",void 0);ue=Le=c([A("media-subnav")],ue);const Sn=g`
  .media-slider-container {
    position: relative;
  }

  .overflow-clip {
    display: none;
    position: absolute;
    top: 0;
    right: 0;
    left: 0;
    height: 0;
    overflow: hidden;
    transition: height 0.2s ease;
  }

  .information-menu {
    position: absolute;
    top: 0;
    right: 0;
    left: 0;
    padding: 0;
    height: 31.9rem;
    overflow-x: hidden;
    font-size: 1.4rem;
    background: var(--mediaSliderBg);
  }

  .open {
    display: block;
  }

  .hidden {
    display: none;
  }

  .info-box {
    padding: 1rem;
  }

  @media (max-width: 889px) {
    .overflow-clip.open {
      display: block;
      height: 35.8rem;
      left: 4rem;
      top: 0;
    }
  }

  @media (min-width: 890px) {
    .overflow-clip {
      display: block;
    }

    .information-menu {
      left: 0;
      z-index: 3;
      height: auto;
      min-height: 21rem;
      background: var(--mediaSliderDesktopBg);
      transform: translate(0, -100%);
      transition: transform 0.2s ease;
    }

    .overflow-clip.open {
      height: 22rem;
    }

    .information-menu.open {
      transform: translate(0, 0);
    }

    .info-box {
      max-width: 100rem;
      padding: 1.5rem 0;
      margin: 0 auto;
    }
  }
`;let ie=class extends P{constructor(){super(...arguments),this.baseHost="",this.config=T,this.mediaSliderOpen=!1,this.menus=yt(),this.selectedMenuOption="texts"}static get styles(){return Sn}updated(e){var t,r;if(e.has("selectedMenuOption")&&this.selectedMenuOption){const n=(r=(t=this.shadowRoot)===null||t===void 0?void 0:t.querySelector(".has-focused"))===null||r===void 0?void 0:r.shadowRoot;if(n){const s=new xt(n,this.selectedMenuOption);this.previousKeydownListener&&this.removeEventListener("keydown",this.previousKeydownListener),this.addEventListener("keydown",s.handleKeyDown),this.previousKeydownListener=s.handleKeyDown}}}shouldUpdate(){const e=this.shadowRoot?this.shadowRoot.querySelector(".information-menu"):null;return e&&(e.scrollTop=0),!0}render(){const e=this.mediaSliderOpen?"open":"closed";return p`
      <div class="media-slider-container">
        <div class="overflow-clip ${e}">
          <div class="information-menu ${e}">
            <div class="info-box">
              <media-subnav
                .baseHost=${this.baseHost}
                .config=${this.config}
                class="${this.selectedMenuOption==="audio"?"has-focused":"hidden"}"
                menu="audio"
                .menuItems=${this.menus.audio}
              ></media-subnav>
              <media-subnav
                .baseHost=${this.baseHost}
                .config=${this.config}
                class="${this.selectedMenuOption==="images"?"has-focused":"hidden"}"
                menu="images"
                .menuItems=${this.menus.images}
              ></media-subnav>
              <media-subnav
                .baseHost=${this.baseHost}
                .config=${this.config}
                class="${this.selectedMenuOption==="software"?"has-focused":"hidden"}"
                menu="software"
                .menuItems=${this.menus.software}
              ></media-subnav>
              <media-subnav
                .baseHost=${this.baseHost}
                .config=${this.config}
                class="${this.selectedMenuOption==="texts"?"has-focused":"hidden"}"
                menu="texts"
                .menuItems=${this.menus.texts}
              ></media-subnav>
              <media-subnav
                .baseHost=${this.baseHost}
                .config=${this.config}
                class="${this.selectedMenuOption==="video"?"has-focused":"hidden"}"
                menu="video"
                .menuItems=${this.menus.video}
              ></media-subnav>
              <media-subnav
                .baseHost=${this.baseHost}
                .config=${this.config}
                class="${this.selectedMenuOption==="web"?"has-focused":"hidden"}"
                menu="web"
                .menuItems=${this.menus.web}
              ></media-subnav>
              <media-subnav
                .baseHost=${this.baseHost}
                .config=${this.config}
                class="${this.selectedMenuOption==="more"?"has-focused":"hidden"}"
                menu="more"
                .menuItems=${this.menus.more}
              ></media-subnav>
            </div>
          </div>
        </div>
      </div>
    `}};c([h({type:String})],ie.prototype,"baseHost",void 0);c([h({type:Object})],ie.prototype,"config",void 0);c([h({type:Boolean})],ie.prototype,"mediaSliderOpen",void 0);c([h({type:Object})],ie.prototype,"menus",void 0);c([h({type:String})],ie.prototype,"selectedMenuOption",void 0);ie=c([A("media-slider")],ie);class Tt extends P{constructor(){super(...arguments),this.active=!1,this.fill="fff"}}c([h({type:Boolean})],Tt.prototype,"active",void 0);c([h({type:String})],Tt.prototype,"fill",void 0);var Te;let ci=Te=class extends Tt{static get styles(){return g`
      svg {
        display: block;
        height: 4rem;
        width: 4rem;
      }
      .fill-color {
        fill: var(--activeColor);
      }
    `}static get closed(){return p`
      <svg
        viewBox="0 0 40 40"
        xmlns="http://www.w3.org/2000/svg"
        aria-labelledby="hamburgerTitleID hamburgerDescID"
      >
        <title id="hamburgerTitleID">Hamburger icon</title>
        <desc id="hamburgerDescID">
          An icon used to represent a menu that can be toggled by interacting
          with this icon.
        </desc>
        <path
          d="m30.5 26.5c.8284271 0 1.5.6715729 1.5 1.5s-.6715729 1.5-1.5 1.5h-21c-.82842712 0-1.5-.6715729-1.5-1.5s.67157288-1.5 1.5-1.5zm0-8c.8284271 0 1.5.6715729 1.5 1.5s-.6715729 1.5-1.5 1.5h-21c-.82842712 0-1.5-.6715729-1.5-1.5s.67157288-1.5 1.5-1.5zm0-8c.8284271 0 1.5.6715729 1.5 1.5s-.6715729 1.5-1.5 1.5h-21c-.82842712 0-1.5-.6715729-1.5-1.5s.67157288-1.5 1.5-1.5z"
          fill="#999"
          fill-rule="evenodd"
        />
      </svg>
    `}static get opened(){return O.close}render(){return this.active?Te.opened:Te.closed}};ci=Te=c([A("icon-hamburger")],ci);const En=g`
  .logged-out-menu {
    background: inherit;
    border: none;
  }
  .logged-out-menu:focus-visible {
    outline: none;
    border: none;
  }
  .dropdown-toggle {
    display: block;
    text-transform: uppercase;
    color: var(--grey80);
    cursor: pointer;
  }

  .dropdown-toggle svg {
    height: 100%;
    width: 4rem;
  }

  .dropdown-toggle .fill-color {
    fill: var(--iconFill);
  }

  .dropdown-toggle:active .fill-color,
  .dropdown-toggle:focus .fill-color,
  .dropdown-toggle:hover .fill-color {
    fill: var(--linkHoverColor);
  }

  .active {
    border-radius: 1rem 1rem 0 0;
    background: var(--activeButtonBg);
  }

  .active .fill-color {
    fill: var(--activeColor);
  }

  span {
    display: none;
    font-size: 1.4rem;
    text-transform: uppercase;
    color: var(--loginTextColor);
  }

  span a {
    color: inherit;
    text-decoration: none;
    outline: 0;
  }

  a:hover,
  a:active,
  a:focus {
    color: var(--linkHoverColor) !important;
    outline: none !important;
    outline-offset: inherit !important;
  }

  @media (min-width: 890px) {
    .logged-out-toolbar {
      padding: 1rem 0.5rem;
      vertical-align: middle;
    }

    .active {
      background: transparent;
    }

    .dropdown-toggle {
      display: inline-block;
      vertical-align: middle;
    }

    .dropdown-toggle svg {
      height: 3rem;
      width: 3rem;
      display: block;
    }

    span {
      display: inline;
      vertical-align: middle;
    }
  }
`;let pe=class extends B{constructor(){super(...arguments),this.baseHost="",this.config=T,this.openMenu="",this.dropdownTabIndex=""}static get styles(){return En}get signupPath(){return L("/account/signup",this.baseHost)}get loginPath(){return L("/login",this.baseHost)}get analyticsEvent(){var e;return`${(e=this.config)===null||e===void 0?void 0:e.eventCategory}|NavLoginIcon`}get menuOpened(){return this.openMenu==="login"}get avatarClass(){return`dropdown-toggle${this.menuOpened?" active":""}`}toggleDropdown(e){e.preventDefault(),this.trackClick(e),this.dropdownTabIndex=this.menuOpened?"":"-1",this.dispatchEvent(new CustomEvent("menuToggled",{bubbles:!0,composed:!0,detail:{menuName:"login"}}))}render(){return p`
      <div class="logged-out-toolbar">
        <button
          class="logged-out-menu ${this.avatarClass}"
          @click=${this.toggleDropdown}
          data-event-click-tracking="${this.analyticsEvent}"
          aria-label="Toggle login menu"
          aria-expanded="${wt(this.menuOpened)}"
        >
          ${O.user}
        </button>
        <span>
          <a href="${this.signupPath}">Sign up</a>
          |
          <a href="${this.loginPath}">Log in</a>
        </span>
      </div>
    `}};c([h({type:String})],pe.prototype,"baseHost",void 0);c([h({type:Object})],pe.prototype,"config",void 0);c([h({type:String})],pe.prototype,"openMenu",void 0);c([S()],pe.prototype,"dropdownTabIndex",void 0);pe=c([A("login-button")],pe);const An=g`
  input[type='text'] {
    color: var(--grey13);
  }

  input:focus {
    outline: none;
  }
  button {
    background: none;
    color: inherit;
    border: none;
    font: inherit;
    cursor: pointer;
  }
  button:focus {
    outline: none;
  }
  .search {
    padding-top: 0;
    margin-right: 0.5rem;
  }
  .search svg {
    position: relative;
    fill: var(--activeSearchColor);
  }
  .search-activated {
    display: -webkit-box;
    display: -ms-flexbox;
    display: flex;
    position: absolute;
    top: 0;
    right: 4rem;
    bottom: 0;
    left: 4rem;
    z-index: 3;
    padding: 0.5rem 0.2rem;
    border-radius: 1rem 1rem 0 0;
    background: var(--searchActiveBg);
  }
  .search-inactive {
    display: none;
  }
  .search-activated .highlight,
  .search-activated .search {
    background: var(--searchActiveInputBg);
    border-radius: 0.5rem;
  }
  .search-activated .highlight {
    display: -webkit-box;
    display: -ms-flexbox;
    display: flex;
    width: 100%;
    margin: 0 0.5rem;
  }
  .search-activated .search {
    height: 100%;
    padding: 0;
    margin-right: 0;
    -ms-flex-item-align: center;
    -ms-grid-row-align: center;
    align-self: center;
  }
  .search-activated .search svg {
    height: 3rem;
    width: 3rem;
  }
  .search-activated .search-field {
    width: 100%;
    height: 100%;
    box-sizing: border-box;
    padding-left: 1rem;
    border-radius: 0.5rem;
    border: none;
    font-size: 1.6rem;
    text-align: center;
  }
  .search-activated .search-field:focus {
    outline: none;
  }
  @keyframes fade-in {
    0% {
      opacity: 0;
    }
    100% {
      opacity: 1;
    }
  }
  .fade-in {
    animation: fade-in 0.2s forwards;
  }

  @media (min-width: 890px) {
    .search svg {
      display: inline;
      width: 2.8rem;
      height: 2.8rem;
      vertical-align: -14px;
    }
    .search path {
      fill: var(--desktopSearchIconFill);
    }

    .search-inactive,
    .search-activated {
      display: block;
      position: static;
      padding: 1.1rem 0.2rem;
      background: transparent;
    }

    .search-activated .highlight {
      width: 13rem;
      height: 2.8rem;
      -webkit-box-orient: horizontal;
      -webkit-box-direction: reverse;
      -ms-flex-direction: row-reverse;
      flex-direction: row-reverse;
    }

    .search-activated .search-field {
      width: calc(100% - 28px);
      height: 100%;
      padding-left: 0;
      font-size: 1.4rem;
      text-align: left;
    }

    .search-activated .search svg {
      width: 2.8rem;
      height: 2.8rem;
    }
  }
`;let D=class extends B{constructor(){super(...arguments),this.baseHost="",this.config=T,this.locationHandler=e=>{},this.open=!1,this.openMenu="",this.searchIn="",this.searchQuery=""}static get styles(){return[An,Lt]}search(e){var t;const r=(t=this.queryInput)===null||t===void 0?void 0:t.value;return r?this.searchIn==="TV"?(this.locationHandler(L(`/details/tv?q=${r}`,this.baseHost)),e.preventDefault(),!1):(this.trackSubmit(e),!0):(e.preventDefault(),!1)}toggleSearchMenu(){this.openMenu!=="search"&&this.dispatchEvent(new CustomEvent("menuToggled",{detail:{menuName:"search"},composed:!0,bubbles:!0}))}get searchInsideInput(){return this.searchIn?p`<input type="hidden" name="sin" value="${this.searchIn}" />`:y}get searchEndpoint(){return"/search"}render(){var e,t;const r=this.open?"flex":"search-inactive";return p`
      <div class="search-activated fade-in ${r}">
        <form
          id="nav-search"
          class="highlight"
          .action=${L(this.searchEndpoint,this.baseHost)}
          method="get"
          @submit=${this.search}
          data-event-submit-tracking="${(e=this.config)===null||e===void 0?void 0:e.eventCategory}|NavSearchSubmit"
        >
          <label for="query" class="sr-only">Search the Archive</label>
          <input
            type="text"
            name="query"
            id="query"
            class="search-field"
            placeholder="Search"
            autocomplete="off"
            value=${this.searchQuery||""}
            @focus=${this.toggleSearchMenu}
          />
          ${this.searchInsideInput}
          <button
            type="submit"
            class="search"
            tabindex="-1"
            data-event-click-tracking="${(t=this.config)===null||t===void 0?void 0:t.eventCategory}|NavSearchClose"
          >
            ${O.search}
          </button>
        </form>
      </div>
    `}};c([h({type:String})],D.prototype,"baseHost",void 0);c([h({type:Object})],D.prototype,"config",void 0);c([h({type:Object})],D.prototype,"locationHandler",void 0);c([h({type:Boolean})],D.prototype,"open",void 0);c([h({type:String})],D.prototype,"openMenu",void 0);c([h({type:String})],D.prototype,"searchIn",void 0);c([h({type:String})],D.prototype,"searchQuery",void 0);c([re("[name=query]")],D.prototype,"queryInput",void 0);D=c([A("nav-search")],D);const kn=g`
  a {
    display: inline-block;
    text-decoration: none;
  }

  .menu-item {
    display: inline-block;
    width: 100%;
    padding: 0;
    font-size: 1.6rem;
    text-align: left;
    background: transparent;
    -webkit-box-align: center;
    -ms-flex-align: center;
    align-items: center;
  }

  .menu-item:focus {
    outline: none;
  }

  .label {
    display: inline-block;
    padding: 0;
    font-weight: 400;
    color: var(--primaryTextColor);
    text-align: left;
    vertical-align: middle;
  }

  .menu-item > .icon {
    display: inline-flex;
    vertical-align: middle;
    -webkit-box-align: center;
    -ms-flex-align: center;
    align-items: center;
    -webkit-box-pack: center;
    -ms-flex-pack: center;
    justify-content: center;
  }

  .menu-item > .icon > svg {
    height: 4rem;
    width: 4rem;
  }

  .menu-item.selected .icon {
    background-color: var(--activeButtonBg);
    border-radius: 1rem 0 0 1rem;
  }

  .icon .fill-color {
    fill: #999;
  }

  .icon.active .fill-color {
    fill: #fff;
  }

  .donate .fill-color {
    fill: #f00;
  }

  @media (min-width: 890px) {
    .menu-item {
      width: auto;
      height: 5rem;
      color: var(--mediaLabelDesktopColor);
      display: inline-flex;
    }
    .menu-item:hover,
    .menu-item:active,
    .menu-item:focus {
      color: var(--linkHoverColor);
    }

    .menu-item:hover .fill-color,
    .menu-item:active .fill-color,
    .menu-item:focus .fill-color {
      fill: var(--linkHoverColor);
    }

    .label {
      display: none;
    }

    .label,
    .web:after {
      padding-right: 1rem;
      font-size: 1.3rem;
      text-transform: uppercase;
      color: inherit;
    }

    .web:after {
      display: none;
      content: 'web';
    }
    .donate,
    .more {
      display: none;
    }

    .menu-item.selected {
      background: var(--activeDesktopMenuIcon);
    }

    .menu-item.selected .label,
    .menu-item.selected.web:after {
      color: var(--linkHoverColor);
    }

    .menu-item.selected .icon {
      background: transparent;
    }

    /* selected state icon colors */
    .web.selected .fill-color {
      fill: #ffcd27;
    }

    .texts.selected .fill-color {
      fill: #faab3c;
    }

    .video.selected .fill-color {
      fill: #f1644b;
    }

    .audio.selected .fill-color {
      fill: #00adef;
    }

    .software.selected .fill-color {
      fill: #9ecc4f;
    }

    .images.selected .fill-color {
      fill: #aa99c9;
    }
  }

  @media (min-width: 1200px) {
    .label,
    .web:after {
      display: inline;
    }

    .web .label {
      display: none;
    }
  }
`;var $t;let U=$t=class extends B{constructor(){super(...arguments),this.config=T,this.icon="",this.href="",this.label="",this.mediatype="",this.openMenu="",this.selected=!1,this.followable=!1}static get styles(){return kn}static get icons(){return O}onClick(e){this.trackClick(e),e.preventDefault(),this.openMenu!=="media"&&this.dispatchMenuToggledEvent(),this.dispatchMediaTypeSelectedEvent()}dispatchMenuToggledEvent(){this.dispatchEvent(new CustomEvent("menuToggled",{bubbles:!0,composed:!0,detail:{menuName:"media"}}))}dispatchMediaTypeSelectedEvent(){this.dispatchEvent(new CustomEvent("mediaTypeSelected",{bubbles:!0,composed:!0,detail:{mediatype:this.mediatype}}))}get buttonClass(){return this.selected?"selected":""}get tooltipPrefix(){return this.selected?"Collapse":"Expand"}get iconClass(){return this.selected?"active":""}get analyticsEvent(){return`${this.config.eventCategory}|NavMenu${he(this.mediatype)}`}get menuItem(){return p`
      <span class="icon ${this.iconClass}">
        ${$t.icons[this.icon]}
      </span>
      <span class="label">${this.label}</span>
    `}render(){return p`
      <a
        href="${this.href}"
        class="menu-item ${this.mediatype} ${this.buttonClass}"
        @click=${this.followable?this.trackClick:this.onClick}
        data-event-click-tracking="${this.analyticsEvent}"
        title="${this.tooltipPrefix} ${this.mediatype} menu"
      >
        ${this.menuItem}
      </a>
    `}};c([h({type:Object})],U.prototype,"config",void 0);c([h({type:String})],U.prototype,"icon",void 0);c([h({type:String})],U.prototype,"href",void 0);c([h({type:String})],U.prototype,"label",void 0);c([h({type:String})],U.prototype,"mediatype",void 0);c([h({type:String})],U.prototype,"openMenu",void 0);c([h({type:Boolean})],U.prototype,"selected",void 0);c([h({type:Boolean})],U.prototype,"followable",void 0);U=$t=c([A("media-button")],U);const In=g`
  :host {
    outline: none;
  }

  .media-menu-inner {
    z-index: -1;
    top: -40rem;
    background-color: var(--mediaMenuBg);
    margin: 0;
    overflow: hidden;
    transition-duration: 0.2s;
    transition-property: top;
    transition-timing-function: ease;
  }

  .menu-group {
    position: relative;
    line-height: normal;
  }

  /* Mobile view styles */
  @media (max-width: 889px) {
    .media-menu-inner {
      position: absolute;
      width: 100%;
    }

    .open .media-menu-inner {
      top: 0;
    }

    .overflow-clip {
      position: absolute;
      z-index: -1; /** needs to be under the navigation, otherwise it intercepts clicks */
      top: 4rem;
      left: 0;
      height: 0;
      width: 100%;
      overflow: hidden;
      transition-duration: 0.2s;
      transition-property: height;
    }

    .open .overflow-clip {
      height: 40rem;
    }
  }

  /* Desktop view styles */
  @media (min-width: 890px) {
    .media-menu-inner {
      display: block;
      position: static;
      width: auto;
      height: 5rem;
      transition-property: none;
    }

    .menu-group {
      font-size: 0;
    }
  }
`,Cn=[{icon:"web",menu:"web",href:"https://web.archive.org",label:"Wayback Machine"},{icon:"texts",menu:"texts",href:"/details/texts",label:"Texts"},{icon:"video",menu:"video",href:"/details/movies",label:"Video"},{icon:"audio",menu:"audio",href:"/details/audio",label:"Audio"},{icon:"software",menu:"software",href:"/details/software",label:"Software"},{icon:"images",menu:"images",href:"/details/image",label:"Images"},{icon:"donate",menu:"donate",href:"/donate/?origin=iawww-mbhmbgrmenu",label:"Donate",followable:!0},{icon:"ellipses",menu:"more",href:"/about/",label:"More"}];let j=class extends P{constructor(){super(...arguments),this.baseHost="",this.config=T,this.openMenu="",this.selectedMenuOption=""}static get styles(){return In}updated(e){var t;if(e.has("currentTab")){const r=Array.from((t=this.mediaButtons)!==null&&t!==void 0?t:[]);r.map((n,s)=>{var o,l,a,d;const u=(o=n.shadowRoot)===null||o===void 0?void 0:o.querySelector("a.menu-item");if(u&&u.classList.contains(`${this.selectedMenuOption}`)){u.classList.remove("selected"),u.blur();const m=((l=this.currentTab)===null||l===void 0?void 0:l.moveTo)==="next"?s+1:s-1;((d=(a=r[m])===null||a===void 0?void 0:a.shadowRoot)===null||d===void 0?void 0:d.querySelector("a.menu-item")).focus()}})}}get mediaMenuOptionsTemplate(){return Cn.map(({icon:t,menu:r,label:n,href:s,followable:o})=>{const l=this.selectedMenuOption===r;return p`
          <media-button
            .config=${this.config}
            .icon=${t}
            .href=${L(s,this.baseHost)}
            ?followable=${o}
            .label=${n}
            .mediatype=${r}
            .openMenu=${this.openMenu}
            .selected=${l}
            .selectedMenuOption=${this.selectedMenuOption}
            data-mediatype="${r}"
          ></media-button>
        `})}get menuOpened(){return this.openMenu==="media"}get menuClass(){return this.menuOpened?"open":"closed"}render(){return p`
      <div class="media-menu-container ${this.menuClass}">
        <div class="overflow-clip">
          <nav class="media-menu-inner" aria-expanded="${this.menuOpened}">
            <div class="menu-group">${this.mediaMenuOptionsTemplate}</div>
          </nav>
        </div>
      </div>
    `}};c([h({type:String})],j.prototype,"baseHost",void 0);c([h({type:Object})],j.prototype,"config",void 0);c([h({type:String})],j.prototype,"openMenu",void 0);c([h({type:String})],j.prototype,"selectedMenuOption",void 0);c([h({type:Object})],j.prototype,"currentTab",void 0);c([Nr("media-button")],j.prototype,"mediaButtons",void 0);j=c([A("media-menu")],j);const xn=Cr`
  <svg class="ia-wordmark stacked" viewBox="0 0 95 30" xmlns="http://www.w3.org/2000/svg">
    <g fill="#fff" fill-rule="evenodd">
      <g transform="translate(0 17)">
        <path d="m3.07645967 2.60391777c.09263226-.31901841.26275495-.97477846.26275495-1.10630359 0-.74950669-.57227138-.76816274-1.19041353-.88009902v-.41183224h2.90678232l3.77209227 12.60682378h-1.60725868l-1.16012991-3.8963154h-3.94266032l-1.15968456 3.8963154h-.95794221zm-.69607795 5.4317081h3.41670507l-1.76223957-5.469953z"/><path d="m13.2661876 2.75363255c0-1.85487748-.0316196-1.85487748-1.469201-2.13611739v-.41183224h3.8032666c1.1601299 0 4.3599508 0 4.3599508 3.35342446 0 2.13518459-1.143652 2.84737918-2.3652399 3.0903742l2.8448791 6.16302512h-1.7773814l-2.7366596-5.82581707h-.9588329v5.82581707h-1.7007817zm1.7007817 3.35295806h.8817878c1.0042583 0 2.411556-.33674165 2.411556-2.43508055 0-2.0237147-.9588329-2.58572812-2.2579112-2.58572812-1.0354326 0-1.0354326.33720805-1.0354326 1.68650665z"/><path d="m30.9386422 12.2878054c-.5250645.2817063-1.3761234.7121946-2.9682403.7121946-3.3552471 0-4.5612478-2.4537366-4.5612478-6.66860403 0-3.4089262.86531-6.18214759 4.3136347-6.18214759 1.3761234 0 2.3812724.28077351 3.0920468.56201342l.0930776 3.12908549h-.4484648c-.0619033-.95612241-.9125167-2.79187744-2.5821241-2.79187744-2.2726076 0-2.6440273 2.94065941-2.6440273 5.28292612 0 2.95931547.8510588 5.61966783 2.9994146 5.61966783 1.4692009 0 2.2267368-.5620135 2.7059306-.9551897z"/><path d="m35.4546446 2.67900836c0-1.48129014-.0926322-1.64966096-1.5466914-1.83622143v-.41276504l3.2470277-.43002189v5.80669465h4.2049699v-5.60101173h1.7007816v12.60682378h-1.7007816v-6.12571303h-4.2049699v6.12571303h-1.7003363z"/><path d="m46.0300434 12.3447063c1.0973359 0 1.3137748-.2807735 1.3137748-1.7420083v-8.05568117c0-1.46123489-.2164389-1.741542-1.3137748-1.741542v-.46919958h4.3287765v.46919958c-1.0826395 0-1.3137748.28030711-1.3137748 1.741542v8.05568117c0 1.4612348.2311353 1.7420083 1.3137748 1.7420083v.4678004h-4.3287765z"/><path d="m55.1990352 2.60391777c-.494781-1.7051627-.6341747-1.7051627-1.8709043-1.98640261v-.41183224h2.9994145l3.0301436 10.26548988h.0307289l2.9535438-10.26548988h1.0042584l-3.771647 12.60682378h-1.344949z"/><path d="m67.7842867 2.75363255c0-1.85487748-.0463161-1.85487748-1.4687557-2.13611739v-.41183224h7.4061269l.0463161 2.77228859h-.433323c0-1.49854698-.6488711-1.89218957-1.6696073-1.89218957h-1.2060007c-.7726777 0-.9735293.07462419-.9735293.97431206v3.7279446h1.8709043c1.0657162 0 1.174381-.82366447 1.2674586-1.44164603h.4324323v3.93316112h-.4324323c0-1.01162415-.3402454-1.61141607-1.2674586-1.61141607h-1.8709043v4.17755538c0 1.0489362.2008516 1.0867147.9735293 1.0867147h1.7168141c.8804518 0 1.2977423-.3749865 1.3908199-1.835755h.433323l-.0472068 2.715854h-6.1685065z"/>
      </g>
      <path d="m1.55575146 9.89647882v-7.6678574c0-1.39053729-.25677019-1.65867172-1.55575146-1.65867172v-.44528687h5.12561364v.44528687c-1.28118091 0-1.55575146.26813443-1.55575146 1.65867172v7.6678574c0 1.39097468.27457055 1.65735948 1.55575146 1.65735948v.4461617h-5.12561364v-.4461617c1.29898127 0 1.55575146-.2663848 1.55575146-1.65735948z"/><path d="m9.92299051 2.28198586c0-1.65823431-.25588017-1.58649851-1.83032201-1.88962601v-.39235985h3.8626781l5.7664266 9.70008019h.0364907v-9.70008019h1.1534634v12h-1.7577856l-6.0405521-10.14492965h-.0369358v10.14492965h-1.15346329z"/><path d="m25.631543.83808413h-1.2633805c-1.4458343 0-1.8668128.44572428-1.977175 1.74834147h-.5122054l.0729815-2.5864256h9.3727795l.0734265 2.63935263h-.5126504c0-1.37304075-.5860768-1.8012685-1.9954203-1.8012685h-1.2442452v11.16191587h-2.0141107z"/><path d="m36.1050097 2.42502005c0-1.7654006-.0556262-1.7654006-1.7399852-2.0326602v-.39235985h8.7684573l.0551811 2.63935263h-.5126504c0-1.42684261-.7685305-1.8012685-1.9762849-1.8012685h-1.4284789c-.9153835 0-1.1534633.0717358-1.1534633.92775388v3.54829773h2.2152547c1.2629356 0 1.3906532-.7851571 1.5010154-1.37391558h.5130954v3.74513378h-.5130954c0-.96274696-.4022881-1.53313407-1.5010154-1.53313407h-2.2152547v3.97608803c0 .9986149.2380798 1.033608 1.1534633 1.033608h2.0319111c1.0439911 0 1.5375061-.356492 1.6469783-1.74702926h.5135404l-.0551812 2.58511336h-7.3034876z"/><path d="m48.1689385 2.42502005c0-1.7654006-.0364908-1.7654006-1.7382052-2.0326602v-.39235985h4.5026011c1.3732977 0 5.1621043 0 5.1621043 3.19180579 0 2.03309761-1.3537173 2.71065102-2.8004416 2.94248013l3.3682731 5.86571408h-2.1053376l-3.2396655-5.54552745h-1.1352179v5.54552745h-2.0141107zm2.0141107 3.19136837h1.0439911c1.189064 0 2.8556227-.32018663 2.8556227-2.31697893 0-1.92680615-1.1352179-2.46132536-2.673169-2.46132536-1.2264448 0-1.2264448.32062404-1.2264448 1.6057447z"/><path d="m61.4610921 2.28198586c0-1.65823431-.2567702-1.58649851-1.830767-1.88962601v-.39235985h3.8622331l5.7659816 9.70008019h.0369357v-9.70008019h1.1534633v12h-1.7568955l-6.0414421-10.14492965h-.0364908v10.14492965h-1.1530183z"/><path d="m75.1550889 2.42502005c0-1.7654006-.0547361-1.7654006-1.7390952-2.0326602v-.39235985h8.7684573l.0551811 2.63935263h-.5126504c0-1.42684261-.7694205-1.8012685-1.9771749-1.8012685h-1.4280339c-.9149385 0-1.1530183.0717358-1.1530183.92775388v3.54829773h2.2148098c1.2633805 0 1.3906531-.7851571 1.5010153-1.37391558h.5130954v3.74513378h-.5130954c0-.96274696-.4022881-1.53313407-1.5010153-1.53313407h-2.2148098v3.97608803c0 .9986149.2380798 1.033608 1.1530183 1.033608h2.0319111c1.0439911 0 1.5379511-.356492 1.6478683-1.74702926h.5126504l-.0551811 2.58511336h-7.3039327z"/><path d="m89.2335734.83808413h-1.2624905c-1.4462793 0-1.8672578.44572428-1.97762 1.74834147h-.5122054l.0734265-2.5864256h9.3718895l.0734265 2.63935263h-.5122054c0-1.37304075-.5856318-1.8012685-1.9958653-1.8012685h-1.2446902v11.16191587h-2.0136657z"/>
    </g>
  </svg>

`,Ln=g`
  button:focus,
  input:focus {
    outline: none;
  }

  nav {
    position: relative;
    display: flex;
    height: 4rem;
    grid-template-areas: 'hamburger empty heart search user';
    -ms-grid-columns: 4rem minmax(1rem, 100%) 4rem 4rem 4rem;
    grid-template-columns: 4rem auto 4rem 4rem 4rem;
    -ms-grid-rows: 100%;
    grid-template-rows: 100%;
    background: var(--primaryNavBg);
    border-bottom: 1px solid var(--primaryNavBottomBorder);
  }

  nav.hide-search {
    grid-template-areas: 'hamburger empty heart user';
    -ms-grid-columns: 4rem minmax(1rem, 100%) 4rem 4rem;
    grid-template-columns: 4rem auto 4rem 4rem;
  }

  .right-side-section {
    display: flex;
    margin-left: auto;
    user-select: none;
  }
  button {
    background: none;
    color: inherit;
    border: none;
    font: inherit;
    cursor: pointer;
  }

  .branding {
    position: static;
    float: left;
    margin: 0 !important;
    padding: 0 5px 0 10px;
    -webkit-transform: translate(0, 0);
    -ms-transform: translate(0, 0);
    transform: translate(0, 0);
  }

  slot,
  .branding {
    display: flex;
    justify-content: left;
    align-items: center;
  }

  media-menu {
    flex: 1;
    justify-self: stretch;
  }

  .ia-logo {
    height: 3rem;
    width: 2.7rem;
    display: inline-block;
  }
  .ia-wordmark {
    height: 3rem;
    width: 9.5rem;
  }
  .ia-logo,
  .ia-wordmark {
    margin-right: 5px;
  }

  .hamburger {
    -ms-grid-row: 1;
    -ms-grid-column: 1;
    grid-area: hamburger;
    padding: 0;
  }
  .hamburger svg {
    height: 4rem;
    width: 4rem;
    fill: var(--activeColor);
  }

  .mobile-donate-link {
    display: inline-block;
  }
  .mobile-donate-link svg {
    height: 4rem;
    width: 4rem;
  }
  .mobile-donate-link .fill-color {
    fill: rgb(255, 0, 0);
  }

  .sr-only {
    position: absolute;
    width: 1px;
    height: 1px;
    margin: -1px;
    padding: 0;
    border: 0;
    overflow: hidden;
    white-space: nowrap;
    clip: rect(1px, 1px, 1px, 1px);
    -webkit-clip-path: inset(50%);
    clip-path: inset(50%);
    user-select: none;
  }

  .search-trigger {
    padding: 0;
  }
  .search-trigger svg {
    height: 4rem;
    width: 4rem;
  }
  .search-trigger .fill-color {
    fill: var(--iconFill);
  }

  .search-activated {
    position: relative;
    z-index: 3;
  }

  .upload {
    display: none;
  }

  .upload span {
    display: none;
  }

  .upload svg {
    height: 3rem;
    width: 3rem;
  }

  .screen-name {
    display: none;
    font-size: 1.3rem;
    vertical-align: middle;
    text-transform: uppercase;
  }

  .user-menu {
    color: var(--lightTextColor);
    padding: 0.5rem;
    height: 100%;
  }

  button.user-menu:hover,
  button.user-menu:focus {
    color: var(--linkHoverColor);
    outline: none;
  }

  .user-menu.active {
    border-radius: 1rem 1rem 0 0;
    background: var(--activeButtonBg);
  }

  .user-menu img {
    display: block;
    width: 3rem;
    height: 3rem;
  }

  .link-home {
    text-decoration: none;
    display: inline-flex;
  }
  a.link-home:focus,
  a.link-home:focus-visible {
    outline-offset: 1px;
  }

  @media only screen and (min-width: 890px) and (max-device-width: 905px) {
    .branding.second-logo {
      padding-right: 0;
    }
  }

  @media (min-width: 906px) {
    .branding.second-logo {
      padding-right: 20px;
    }
  }

  @media (max-width: 889px) {
    slot[name='opt-sec-logo'] {
      display: none;
    }

    .right-side-section {
      display: initial;
    }
    .right-side-section .user-info {
      float: right;
    }
  }

  @media (min-width: 890px) {
    :host {
      --userIconWidth: 3.2rem;
      --userIconHeight: 3.2rem;
    }

    nav {
      display: flex;
      z-index: 4;
      height: 5rem;
      padding-right: 1.5rem;
    }

    slot[name='opt-sec-logo-mobile'] {
      display: none;
    }

    .ia-logo,
    .ia-wordmark {
      margin-right: 10px;
    }

    .hamburger,
    .search-trigger,
    .mobile-donate-link {
      display: none;
    }

    .user-info {
      display: block;
      float: right;
      vertical-align: middle;
      height: 100%;
    }

    .user-info .user-menu img {
      height: 3rem;
      width: 3rem;
      margin-right: 0.5rem;
    }

    .user-menu {
      padding: 1rem 0.5rem;
    }
    .user-menu.active {
      background: transparent;
    }

    .user-menu img {
      display: inline-block;
      vertical-align: middle;
      margin-right: 0.5rem;
    }

    .upload {
      display: block;
      padding: 1rem 0.5rem;
      float: right;
      font-size: 1.4rem;
      text-transform: uppercase;
      text-decoration: none;
      color: var(--lightTextColor);
    }
    .upload:active,
    .upload:focus,
    .upload:hover {
      color: var(--linkHoverColor);
    }
    .upload:focus-visible {
      outline: none;
    }

    .upload svg {
      vertical-align: middle;
      fill: var(--iconFill);
    }

    .upload:hover svg,
    .upload:focus svg,
    .upload:active svg {
      fill: var(--linkHoverColor);
    }

    nav-search {
      float: right;
      margin-left: 1rem;
    }
  }

  @media (min-width: 990px) {
    .screen-name {
      display: inline-block;
      text-overflow: ellipsis;
      overflow: hidden;
      white-space: nowrap;
      max-width: 165px;
    }

    .upload span {
      display: inline-block;
      vertical-align: middle;
    }
  }
`;function Tn(i){window.location.href=i}let I=class extends B{constructor(){super(...arguments),this.mediaBaseHost="https://archive.org",this.baseHost="",this.hideSearch=!1,this.config=T,this.openMenu="",this.screenName="",this.searchIn="",this.searchQuery="",this.secondIdentitySlotMode="",this.selectedMenuOption="",this.signedOutMenuOpen=!1,this.userMenuOpen=!1,this.mediaMenuAnimate=!1,this.username="",this.userProfileImagePath=""}static get styles(){return Ln}toggleMediaMenu(e){this.trackClick(e),this.dispatchEvent(new CustomEvent("menuToggled",{detail:{menuName:"media"}}))}toggleSearchMenu(e){this.trackClick(e),this.dispatchEvent(new CustomEvent("menuToggled",{detail:{menuName:"search"}}))}toggleUserMenu(e){this.trackClick(e),this.dispatchEvent(new CustomEvent("menuToggled",{detail:{menuName:"user"}}))}updated(e){var t,r,n,s,o,l,a,d,u,m,v,b,x,C,We,je;if(e.has("currentTab")){if(!this.currentTab||Object.keys(this.currentTab).length===0)return y;if(this.currentTab&&this.currentTab.mediatype==="usermenu"){const Zi=Array.from((s=(n=(r=(t=this.shadowRoot)===null||t===void 0?void 0:t.querySelector("media-menu"))===null||r===void 0?void 0:r.shadowRoot)===null||n===void 0?void 0:n.querySelectorAll("media-button"))!==null&&s!==void 0?s:[]).filter(Xi=>{var Ke,Ge;return(Ge=(Ke=Xi.shadowRoot)===null||Ke===void 0?void 0:Ke.querySelector("a"))===null||Ge===void 0?void 0:Ge.classList.contains("images")});let qe;this.username?qe=(o=this.shadowRoot)===null||o===void 0?void 0:o.querySelector("a.upload"):qe=(d=(a=(l=this.shadowRoot)===null||l===void 0?void 0:l.querySelector("login-button"))===null||a===void 0?void 0:a.shadowRoot)===null||d===void 0?void 0:d.querySelector("span a");const Qi=(m=(u=Zi[0])===null||u===void 0?void 0:u.shadowRoot)===null||m===void 0?void 0:m.querySelector("a.menu-item"),Bt=this.currentTab.moveTo==="next"?qe:Qi;Bt&&Bt.focus()}else this.currentTab.moveTo==="next"&&(!((v=this.shadowRoot)===null||v===void 0)&&v.querySelector(".user-menu")?((b=this.shadowRoot)===null||b===void 0?void 0:b.querySelector(".user-menu")).focus():(je=(We=(C=(x=this.shadowRoot)===null||x===void 0?void 0:x.querySelector("login-button"))===null||C===void 0?void 0:C.shadowRoot)===null||We===void 0?void 0:We.querySelectorAll("span a")[0])===null||je===void 0||je.focus())}}get userIcon(){var e;const t=this.openMenu==="user"?"active":"",r=this.openMenu==="user"?"Close user menu":"Expand user menu";return p`
      <button
        class="user-menu ${t}"
        title="${r}"
        @click="${this.toggleUserMenu}"
        data-event-click-tracking="${(e=this.config)===null||e===void 0?void 0:e.eventCategory}|NavUserMenu"
      >
        <img
          src="${this.mediaBaseHost}${this.userProfileImagePath}"
          alt="Profile picture for ${this.screenName}"
        />
        <span class="screen-name" dir="auto">${this.screenName}</span>
      </button>
    `}get loginIcon(){return p`
      <login-button
        .baseHost=${this.baseHost}
        .config=${this.config}
        .dropdownOpen=${this.signedOutMenuOpen}
        .openMenu=${this.openMenu}
        @signedOutMenuToggled=${this.signedOutMenuToggled}
      ></login-button>
    `}get searchMenuOpen(){return this.openMenu==="search"}get allowSecondaryIcon(){return this.secondIdentitySlotMode==="allow"}get searchMenu(){var e;return this.hideSearch?y:p`
      <button
        class="search-trigger"
        @click="${this.toggleSearchMenu}"
        data-event-click-tracking="${(e=this.config)===null||e===void 0?void 0:e.eventCategory}|NavSearchOpen"
      >
        ${O.search}
      </button>
      <nav-search
        .baseHost=${this.baseHost}
        .config=${this.config}
        .locationHandler=${Tn}
        .open=${this.searchMenuOpen}
        .openMenu=${this.openMenu}
        .searchIn=${this.searchIn}
        .searchQuery=${this.searchQuery}
        @blur=${this.emitNavSearchBlurEvent}
      ></nav-search>
    `}emitNavSearchBlurEvent(e){var t;const r=e.relatedTarget,n=r==null?void 0:r.classList.contains("upload");n&&((t=this.shadowRoot)===null||t===void 0?void 0:t.querySelector("a.upload")).focus(),this.dispatchEvent(new CustomEvent("navSearchBlur",{detail:{isUploadButton:!!n},bubbles:!0,composed:!0}))}get mobileDonateHeart(){return p`
      <a
        class="mobile-donate-link"
        .href=${L("/donate/?origin=iawww-mbhrt",this.baseHost)}
      >
        ${O.donateUnpadded}
        <span class="sr-only">"Donate to the archive"</span>
      </a>
    `}get uploadButtonTemplate(){return p` <a
      .href="${L("/upload",this.baseHost)}"
      class="upload"
      @focus=${this.toggleMediaMenu}
    >
      ${O.upload}
      <span>Upload</span>
    </a>`}get userStateTemplate(){return p`<div class="user-info">
      ${this.username?this.userIcon:this.loginIcon}
    </div>`}get secondLogoSlot(){return this.allowSecondaryIcon?p`
          <slot name="opt-sec-logo"></slot>
          <slot name="opt-sec-logo-mobile"></slot>
        `:y}get secondLogoClass(){return this.allowSecondaryIcon?"second-logo":""}render(){var e,t;return p`
      <nav class=${this.hideSearch?"hide-search":""}>
        <button
          class="hamburger"
          @click="${this.toggleMediaMenu}"
          data-event-click-tracking="${(e=this.config)===null||e===void 0?void 0:e.eventCategory}|NavHamburger"
          title="Open main menu"
        >
          <icon-hamburger ?active=${this.openMenu==="media"}></icon-hamburger>
        </button>

        <div class=${`branding ${this.secondLogoClass}`}>
          <a
            .href=${L("/",this.baseHost)}
            @click=${this.trackClick}
            data-event-click-tracking="${(t=this.config)===null||t===void 0?void 0:t.eventCategory}|NavHome"
            title="Go home"
            class="link-home"
            >${O.iaLogo}${xn}</a
          >
          ${this.secondLogoSlot}
        </div>
        <media-menu
          .baseHost=${this.baseHost}
          .config=${this.config}
          ?mediaMenuAnimate=${this.mediaMenuAnimate}
          .selectedMenuOption=${this.selectedMenuOption}
          .openMenu=${this.openMenu}
          .currentTab=${this.currentTab}
        ></media-menu>
        <div class="right-side-section">
          ${this.mobileDonateHeart} ${this.userStateTemplate}
          ${this.uploadButtonTemplate} ${this.searchMenu}
        </div>
      </nav>
    `}};c([h({type:String})],I.prototype,"mediaBaseHost",void 0);c([h({type:String})],I.prototype,"baseHost",void 0);c([h({type:Boolean})],I.prototype,"hideSearch",void 0);c([h({type:Object})],I.prototype,"config",void 0);c([h({type:String})],I.prototype,"openMenu",void 0);c([h({type:String})],I.prototype,"screenName",void 0);c([h({type:String})],I.prototype,"searchIn",void 0);c([h({type:String})],I.prototype,"searchQuery",void 0);c([h({type:String})],I.prototype,"secondIdentitySlotMode",void 0);c([h({type:String})],I.prototype,"selectedMenuOption",void 0);c([h({type:Boolean})],I.prototype,"signedOutMenuOpen",void 0);c([h({type:Boolean})],I.prototype,"userMenuOpen",void 0);c([h({type:Boolean})],I.prototype,"mediaMenuAnimate",void 0);c([h({type:String})],I.prototype,"username",void 0);c([h({type:String})],I.prototype,"userProfileImagePath",void 0);c([h({type:Object})],I.prototype,"currentTab",void 0);I=c([A("primary-nav")],I);const Pn=g`
  .menu-wrapper {
    position: relative;
  }

  button:focus,
  input:focus {
    outline-color: var(--linkColor);
    outline-width: 0.16rem;
    outline-style: auto;
    outline-offset: 2px !important;
  }
  .search-menu-inner {
    position: absolute;
    right: 0;
    left: 0;
    z-index: 4;
    padding: 0 4.5rem;
    font-size: 1.6rem;
    background-color: var(--searchMenuBg);
  }
  .tx-slide {
    overflow: hidden;
    transition-property: top;
    transition-duration: 0.2s;
    transition-timing-function: ease;
  }
  .initial,
  .closed {
    top: var(--topOffset, -1500px);
  }
  .closed {
    transition-duration: 0.2s;
  }

  label,
  a {
    padding: 1rem;
    display: block;
  }

  .advanced-search {
    text-decoration: underline;
    color: inherit;
    line-height: normal;
    padding: 0.5rem;
    margin-top: 5px;
  }

  .advanced-search:hover {
    text-decoration: none;
  }

  @media (min-width: 890px) {
    .search-menu-inner {
      overflow: visible;
      right: 2rem;
      left: auto;
      z-index: 5;
      padding: 1rem 2rem;
      transition: opacity 0.2s ease-in-out;
      font-size: 1.4rem;
      color: var(--inverseTextColor);
      border-radius: 2px;
      background: var(--primaryTextColor);
      box-shadow: 0 1px 2px 1px rgba(0, 0, 0, 0.15);
    }

    .search-menu-inner:after {
      position: absolute;
      right: 7px;
      top: -7px;
      width: 12px;
      height: 7px;
      box-sizing: border-box;
      color: #fff;
      content: '';
      border-bottom: 7px solid currentColor;
      border-left: 6px solid transparent;
      border-right: 6px solid transparent;
    }

    .advanced-search {
      text-decoration: none;
      color: var(--linkColor);
    }

    .advanced-search:hover {
      text-decoration: underline;
    }

    .initial,
    .closed {
      opacity: 0;
      transition-duration: 0.2s;
    }

    .open {
      opacity: 1;
    }

    label {
      padding: 0;
      font-weight: normal;
      margin: 0;
    }

    label + label {
      padding-top: 7px;
    }

    a {
      padding: 1rem 0 0 0;
    }
  }
`;let V=class extends B{constructor(){super(...arguments),this.baseHost="",this.config=T,this.hideSearch=!1,this.openMenu="",this.searchMenuOpen=!1,this.searchMenuAnimate=!1,this.selectedSearchType=""}static get styles(){return Pn}firstUpdated(){var e;(e=this.shadowRoot)===null||e===void 0||e.addEventListener("keydown",t=>this.handleKeyDownEvent(t))}disconnectedCallback(){var e;(e=this.shadowRoot)===null||e===void 0||e.removeEventListener("keydown",t=>this.handleKeyDownEvent(t))}handleKeyDownEvent(e){if(!this.shadowRoot)return;const t=this.shadowRoot.querySelectorAll(".search-menu-inner label input[type=radio]"),r=t.length-1;if(!r)return;const n=s=>{e.preventDefault();const o=t[s];o.checked=!0,o.dispatchEvent(new Event("change")),o.focus()};e.key==="Home"?n(0):e.key==="End"&&n(r)}selectSearchType(e){const t=e.target;this.selectedSearchType=t.value}searchInChanged(e){const t=e.target;this.dispatchEvent(new CustomEvent("searchInChanged",{detail:{searchIn:t.value}}))}get searchTypesTemplate(){return[{label:"metadata",value:"",isDefault:!0},{label:"text contents",value:"TXT"},{label:"TV news captions",value:"TV"},{label:"radio transcripts",value:"RADIO"},{label:"archived web sites",value:"WEB"}].map(({value:t,label:r,isDefault:n})=>this.config.hiddenSearchOptions&&this.config.hiddenSearchOptions.includes(t)?p``:p`
        <label @click="${this.selectSearchType}">
          <input
            form="nav-search"
            type="radio"
            name="sin"
            value="${t}"
            ?checked=${n}
            ?disabled=${!this.openMenu}
            @change=${this.searchInChanged}
          />
          Search ${r}
        </label>
      `)}get menuClass(){return this.openMenu==="search"?"open":"closed"}render(){return this.hideSearch?p``:p`
      <div class="menu-wrapper">
        <div
          class="search-menu-inner tx-slide ${this.menuClass}"
          aria-hidden="${!this.openMenu}"
        >
          ${this.searchTypesTemplate}
          <a
            class="advanced-search"
            href="${L("/advancedsearch.php",this.baseHost)}"
            @click=${this.trackClick}
            tabindex=${this.openMenu?"0":"-1"}
            data-event-click-tracking="${this.config.eventCategory}|NavAdvancedSearch"
            >Advanced Search</a
          >
        </div>
      </div>
    `}};c([h({type:String})],V.prototype,"baseHost",void 0);c([h({type:Object})],V.prototype,"config",void 0);c([h({type:Boolean})],V.prototype,"hideSearch",void 0);c([h({type:String})],V.prototype,"openMenu",void 0);c([h({type:Boolean})],V.prototype,"searchMenuOpen",void 0);c([h({type:Boolean})],V.prototype,"searchMenuAnimate",void 0);c([h({type:String})],V.prototype,"selectedSearchType",void 0);V=c([A("search-menu")],V);const Mn=g`
  @media (min-width: 890px) {
    .initial,
    .closed,
    .open {
      right: 33.5rem;
    }

    .search-hidden.initial,
    .search-hidden.closed,
    .search-hidden.open {
      right: 18.5rem;
    }
  }

  @media (min-width: 990px) {
    .initial,
    .closed,
    .open {
      right: 40rem;
    }

    .search-hidden.initial,
    .search-hidden.closed,
    .search-hidden.open {
      right: 26rem;
    }
  }
`;let di=class extends R{static get styles(){return[It,Mn]}};di=c([A("signed-out-dropdown")],di);const Rn=g`
  :host {
    --white: #fff;
    --grey13: #222;
    --grey20: #333;
    --grey40: #666;
    --grey28: #474747;
    --grey60: #999;
    --grey66: #aaa;
    --grey80: #ccc;
    --greya0: #a0a0a0;
    --grey6f: #6f6f6f;
    --errorYellow: #ffcd27;

    --linkColor: #4b64ff;
    --linkHoverColor: var(--white);
    --subnavLinkColor: var(--grey66);
    --primaryTextColor: var(--white);
    --inverseTextColor: var(--grey20);
    --lightTextColor: var(--grey60);
    --activeColor: var(--white);
    --activeButtonBg: var(--grey20);
    --iconFill: var(--grey60);

    --searchActiveBg: var(--grey20);
    --searchActiveInputBg: var(--white);
    --searchMenuBg: var(--grey20);
    --desktopSearchIconFill: var(--grey20);

    --mediaMenuBg: var(--grey13);
    --mediaLabelDesktopColor: var(--grey60);
    --activeDesktopMenuIcon: var(--grey28);

    --mediaSliderBg: var(--grey20);
    --mediaSliderDesktopBg: var(--grey28);

    --primaryNavBg: var(--grey13);
    --primaryNavBottomBorder: var(--grey20);

    --desktopSubnavBg: var(--grey20);

    --dropdownMenuBg: var(--grey20);
    --dropdownMenuInfoItem: var(--greya0);
    --dropdownMenuDivider: var(--grey40);
    --inverseDropdownMenuInfoItem: var(--grey6f);

    --loginTextColor: var(--grey60);

    --themeFontFamily: 'Helvetica Neue', Helvetica, Arial, sans-serif;
    --logoWidthTablet: 263px;

    --savePageSubmitBg: var(--grey13);
    --savePageSubmitText: var(--white);
    --savePageInputBorder: var(--grey60);
    --savePageErrorText: var(--errorYellow);

    color: var(--primaryTextColor);
    font-family: var(--themeFontFamily);
  }

  primary-nav:focus {
    outline: none !important;
  }

  #close-layer {
    display: none;
    position: fixed;
    top: 0;
    right: 0;
    bottom: 0;
    left: 0;
    z-index: 3;
  }
  #close-layer.visible {
    display: block;
  }

  .topnav {
    position: relative;
    z-index: 4;
  }

  @media (max-width: 889px) {
    desktop-subnav {
      display: none;
    }
  }
`,Bn=g`
  @media (min-width: 890px) {
    .initial,
    .closed,
    .open {
      right: 22.4rem;
    }

    .search-hidden.initial,
    .search-hidden.closed,
    .search-hidden.open {
      right: 5.8rem;
    }
  }

  @media (min-width: 990px) {
    .initial,
    .closed,
    .open {
      right: 27.2rem;
    }

    .search-hidden.initial,
    .search-hidden.closed,
    .search-hidden.open {
      right: 12rem;
    }
  }
`;let ze=class extends R{constructor(){super(...arguments),this.username="",this.screenName=""}static get styles(){return[It,Bn]}render(){return p`
      <div class="nav-container">
        <nav
          class="${this.menuClass}"
          aria-hidden=${!this.open}
          aria-expanded=${this.open}
        >
          <h3>${this.screenName}</h3>
          <ul>
            ${this.dropdownItems}
          </ul>
        </nav>
      </div>
    `}};c([h({type:String})],ze.prototype,"username",void 0);c([h({type:String})],ze.prototype,"screenName",void 0);ze=c([A("user-menu")],ze);let E=class extends P{constructor(){super(...arguments),this.localLinks=!1,this.waybackPagesArchived="",this.baseHost="https://archive.org",this.mediaBaseHost="https://archive.org",this.admin=!1,this.canManageFlags=!1,this.config=T,this.hideSearch=!1,this.itemIdentifier="",this.mediaSliderOpen=!1,this.openMenu="",this.screenName="",this.searchIn="",this.searchQuery="",this.selectedMenuOption="",this.username="",this.userProfileImagePath="/services/img/user/profile",this.secondIdentitySlotMode="",this.menus=yt()}get normalizedBaseHost(){return this.localLinks?"":this.baseHost}static get styles(){return Rn}updated(e){var t;if((e.has("username")||e.has("waybackPagesArchived")||e.has("itemIdentifier")||e.has("localLinks")||e.has("baseHost"))&&this.menuSetup(),this.openMenu==="search"){const r=(t=this.renderRoot.querySelector("search-menu"))===null||t===void 0?void 0:t.shadowRoot;r&&(this.keyboardNavigation=new xt(r,this.openMenu),this.previousKeydownListener&&this.removeEventListener("keydown",this.previousKeydownListener),this.addEventListener("keydown",this.keyboardNavigation.handleKeyDown),this.previousKeydownListener=this.keyboardNavigation.handleKeyDown)}}firstUpdated(){document.addEventListener("keydown",e=>{e.key==="Escape"&&(this.openMenu="",this.mediaSliderOpen=!1)},!1)}menuSetup(){this.menus=yt(this.username,this.normalizedBaseHost,this.waybackPagesArchived,this.itemIdentifier)}menuToggled(e){const t=this.openMenu;this.openMenu=t===e.detail.menuName?"":e.detail.menuName,this.openMenu!=="media"&&this.closeMediaSlider()}navSearchBlurEvent(e){var t,r;if(this.previousKeydownListener&&this.removeEventListener("keydown",this.previousKeydownListener),!((t=e.detail)===null||t===void 0?void 0:t.isUploadButton)){const s=this.renderRoot.querySelector("search-menu"),o=(r=this.keyboardNavigation)===null||r===void 0?void 0:r.getFocusableElements();s&&(o!=null&&o.length)&&o[0].focus()}}openMediaSlider(){this.mediaSliderOpen=!0}closeMediaSlider(){this.mediaSliderOpen=!1,this.selectedMenuOption=""}closeMenus(){this.openMenu="",this.closeMediaSlider()}searchInChanged(e){this.searchIn=e.detail.searchIn}trackClick(e){this.dispatchEvent(new CustomEvent("analyticsClick",{bubbles:!0,composed:!0,detail:e.detail}))}trackSubmit(e){this.dispatchEvent(new CustomEvent("analyticsSubmit",{bubbles:!0,composed:!0,detail:e.detail}))}mediaTypeSelected(e){if(this.selectedMenuOption===e.detail.mediatype){this.closeMediaSlider();return}this.selectedMenuOption=e.detail.mediatype,this.openMediaSlider()}get searchMenuOpened(){return this.openMenu==="search"}get signedOutOpened(){return this.openMenu==="login"}get userMenuOpened(){return this.openMenu==="user"}get searchMenuTabIndex(){return this.searchMenuOpened?"":"-1"}get userMenuTabIndex(){return this.userMenuOpened?"":"-1"}get signedOutTabIndex(){return this.signedOutOpened?"":"-1"}get closeLayerClass(){return this.openMenu||this.mediaSliderOpen?"visible":""}get userMenu(){return p`
      <user-menu
        .baseHost=${this.normalizedBaseHost}
        .config=${this.config}
        .menuItems=${this.userMenuItems}
        ?open=${this.openMenu==="user"}
        .username=${this.username}
        ?hideSearch=${this.hideSearch}
        tabindex="${this.userMenuTabIndex}"
        @menuToggled=${this.menuToggled}
        @trackClick=${this.trackClick}
        @focusToOtherMenuItem=${e=>this.currentTab=e.detail}
      ></user-menu>
    `}get signedOutDropdown(){return p`
      <signed-out-dropdown
        .baseHost=${this.normalizedBaseHost}
        .config=${this.config}
        .open=${this.signedOutOpened}
        ?hideSearch=${this.hideSearch}
        tabindex="${this.signedOutTabIndex}"
        .menuItems=${this.signedOutMenuItems}
        @focusToOtherMenuItem=${e=>{this.currentTab=e.detail}}
      ></signed-out-dropdown>
    `}get signedOutMenuItems(){return this.menus.signedOut}get userMenuItems(){const e=this.menus.user;let t=this.menus.userAdmin;return this.canManageFlags&&(t=t.concat(this.menus.userAdminFlags)),this.itemIdentifier&&this.admin?[e,t]:[e]}get allowSecondaryIcon(){return this.secondIdentitySlotMode==="allow"}get secondLogoSlot(){return this.allowSecondaryIcon?p`
          <slot name="opt-sec-logo" slot="opt-sec-logo"></slot>
          <slot name="opt-sec-logo-mobile" slot="opt-sec-logo-mobile"></slot>
        `:y}get separatorTemplate(){return p`<li class="divider" role="presentation"></li>`}render(){return p`
      <div class="topnav">
        <primary-nav
          .baseHost=${this.normalizedBaseHost}
          .mediaBaseHost=${this.mediaBaseHost}
          .config=${this.config}
          .openMenu=${this.openMenu}
          .screenName=${this.screenName}
          .searchIn=${this.searchIn}
          .searchQuery=${this.searchQuery}
          .secondIdentitySlotMode=${this.secondIdentitySlotMode}
          .selectedMenuOption=${this.selectedMenuOption}
          .username=${this.username}
          .userProfileImagePath=${this.userProfileImagePath}
          .currentTab=${this.currentTab}
          ?hideSearch=${this.hideSearch}
          @mediaTypeSelected=${this.mediaTypeSelected}
          @trackClick=${this.trackClick}
          @trackSubmit=${this.trackSubmit}
          @menuToggled=${this.menuToggled}
          @navSearchBlur=${this.navSearchBlurEvent}
        >
          ${this.secondLogoSlot}
        </primary-nav>
        <media-slider
          .baseHost=${this.normalizedBaseHost}
          .config=${this.config}
          .selectedMenuOption=${this.selectedMenuOption}
          .mediaSliderOpen=${this.mediaSliderOpen}
          .menus=${this.menus}
          tabindex="${this.mediaSliderOpen?"1":"-1"}"
          @focusToOtherMenuItem=${e=>this.currentTab=e.detail}
        ></media-slider>
      </div>
      ${this.username?this.userMenu:this.signedOutDropdown}
      <search-menu
        .baseHost=${this.normalizedBaseHost}
        .config=${this.config}
        .openMenu=${this.openMenu}
        tabindex="${this.searchMenuTabIndex}"
        ?hideSearch=${this.hideSearch}
        @searchInChanged=${this.searchInChanged}
        @trackClick=${this.trackClick}
        @trackSubmit=${this.trackSubmit}
      ></search-menu>
      <desktop-subnav
        .baseHost=${this.normalizedBaseHost}
        .menuItems=${this.menus.more.links}
        @focus=${this.closeMenus}
      ></desktop-subnav>
      <div
        id="close-layer"
        class="${this.closeLayerClass}"
        @click=${this.closeMenus}
      ></div>
    `}};c([h({type:Boolean})],E.prototype,"localLinks",void 0);c([h({type:String})],E.prototype,"waybackPagesArchived",void 0);c([h({type:String})],E.prototype,"baseHost",void 0);c([h({type:String})],E.prototype,"mediaBaseHost",void 0);c([h({type:Boolean})],E.prototype,"admin",void 0);c([h({type:Boolean})],E.prototype,"canManageFlags",void 0);c([h({type:Object})],E.prototype,"config",void 0);c([h({type:Boolean})],E.prototype,"hideSearch",void 0);c([h({type:String})],E.prototype,"itemIdentifier",void 0);c([h({type:Boolean})],E.prototype,"mediaSliderOpen",void 0);c([h({type:String})],E.prototype,"openMenu",void 0);c([h({type:String})],E.prototype,"screenName",void 0);c([h({type:String})],E.prototype,"searchIn",void 0);c([h({type:String})],E.prototype,"searchQuery",void 0);c([h({type:String})],E.prototype,"selectedMenuOption",void 0);c([h({type:String})],E.prototype,"username",void 0);c([h({type:String})],E.prototype,"userProfileImagePath",void 0);c([h({type:String})],E.prototype,"secondIdentitySlotMode",void 0);c([h({type:Object})],E.prototype,"currentTab",void 0);c([S()],E.prototype,"menus",void 0);E=c([A("ia-topnav")],E);class ct{constructor(e){this.generator=e.generator}get(){return si(this,void 0,void 0,function*(){return this.cachedResponse?this.cachedResponse:this.previousPromise?(this.previousPromise=this.previousPromise.then(e=>e),this.previousPromise):(this.previousPromise=this.generateSingletonAndCache(),this.previousPromise)})}reset(){this.cachedResponse=void 0,this.previousPromise=void 0}generateSingletonAndCache(){return si(this,void 0,void 0,function*(){const e=yield this.generator();return this.cachedResponse=e,e})}}const On="**",Dn="\\.\\.\\/",Un=/:([^\\/]+)/g,Ri="router-slot",me=window,dt="native",Nn="prefix";function zn(i,e){i.dispatchEvent(new CustomEvent("changestate",{detail:e}))}function z(i,e){me.dispatchEvent(new CustomEvent(i,{detail:e}))}function Q(i,e,t,r){const n=Array.isArray(e)?e:[e];return n.forEach(s=>i.addEventListener(s,t,r)),()=>n.forEach(s=>i.removeEventListener(s,t,r))}function Bi(i){i.forEach(e=>e())}const Vn=[["pushState",["pushstate","changestate"]],["replaceState",["replacestate","changestate"]],["forward",["pushstate","changestate"]],["go",["pushstate","changestate"]],["back",["popstate"]]];function Hn(){for(const[i,e]of Vn)for(const t of e)Fn(history,i,()=>z(t));window.addEventListener("popstate",i=>{if(Oi()){i.preventDefault(),i.stopPropagation();return}setTimeout(()=>z("changestate"),0)})}function Fn(i,e,t){const r=i[e];Wn(i,e,r),i[e]=(...n)=>{Oi()||(r.apply(i,n),t(n))}}function Wn(i,e,t){i[dt]==null&&(i[dt]={}),i[dt][`${e}`]=t.bind(i)}function Oi(){return!me.dispatchEvent(new CustomEvent("willchangestate",{cancelable:!0}))}const hi=document.createElement("a");function Di(i={}){return Ie(window.location.pathname,i)}function jn(i={}){return Ie(Kn(Di(),qn()),i)}function qn(i={}){return Ui(".",i)}function Ui(i,e={}){return hi.href=i,Ie(hi.pathname,e)}function Kn(i,e){return i.replace(new RegExp(`^${e}`),"")}function Ni(){return window.location.search}function ye(i){return Ie(i,{start:!1,end:!1})}function Ie(i,{start:e=!0,end:t=!0}={}){return i=e&&!i.startsWith("/")?`/${i}`:!e&&i.startsWith("/")?i.slice(1):i,t&&!i.endsWith("/")?`${i}/`:!t&&i.endsWith("/")?i.slice(0,i.length-1):i}function Gn(i,e=Di()){return new RegExp(`^${ye(i)}(/|$)`,"gm").test(ye(e))}function Yn(i,e){const t=[],r=ye(i.path.replace(Un,(o,...l)=>(t.push(l[0]),"([^/]+)"))),n=i.path===On||i.path.length===0&&i.pathMatch!="full"?/^/:(()=>{switch(i.pathMatch||Nn){case"full":return new RegExp(`^/?${r}/?$`);case"suffix":return new RegExp(`^.*?${r}/?$`);case"fuzzy":return new RegExp(`^.*?${r}.*?$`);case"prefix":default:return new RegExp(`^[/]?${r}(?:/|$)`)}})(),s=e.match(n);if(s!=null){const o=t.reduce((d,u,m)=>(d[u]=s[m+1],d),{}),l=ye(e.slice(0,s[0].length)),a=ye(e.slice(s[0].length,e.length));return{route:i,match:s,params:o,fragments:{consumed:l,rest:a}}}return null}function Zn(i,e){for(const t of i){const r=Yn(t,e);if(r!=null)return r}return null}async function Qn(i,e){let t=i.component;if(t instanceof Function)try{t=t()}catch(s){if(!(s instanceof TypeError))throw s}const r=await Promise.resolve(t);let n;return r instanceof HTMLElement?n=t:n=new(r.default?r.default:r),i.setup!=null&&i.setup(n,e),n}function Xn(i){return"redirectTo"in i}function Jn(i){return"resolve"in i}function es(i){let e=[i];for(;i.parent!=null;)i=i.parent,e.push(i);const t=e.reduce((n,s)=>({slot:s,child:n}),void 0),r=e.length;return{tree:t,depth:r}}function ts(i,e){let t=i;const r=[];for(;t!=null&&t.slot.match!=null&&e>0;)r.push(t.slot.match.fragments.consumed),t=t.child,e--;return r}function zi(i,e=""){const{tree:t,depth:r}=es(i);if(!e.startsWith("/")){let n=0;e.startsWith("./")&&(e=e.slice(2));const s=e.match(new RegExp(Dn,"g"));if(s!=null){n=s.length;const l=s.reduce((a,d)=>a+d.length,0);e=e.slice(l)}const o=ts(t,r-1-n).filter(l=>l.length>0);e=`${o.join("/")}${o.length>0?"/":""}${e}`}return Ui(e,{end:!1})}function is(i,e){history.replaceState(history.state,"",`${zi(i,e.redirectTo)}${e.preserveQuery?Ni():""}`)}function rs(i,e){if(i==null)return!0;const{route:t,fragments:r}=i,{route:n,fragments:s}=e,o=t==n;return!(r.consumed==s.consumed)||!o}function Vi(i){return Hi(i,Ri)}function Hi(i,e,t=0,r=0){const n=i.getRootNode();if(r>=t){const o=n.querySelector(e);if(o!=null&&o!=i)return o}const s=n.getRootNode();return s.host==null?null:Hi(s.host,e,t,++r)}class ns{constructor(e){this.routerSlot=e}setup(){this.boundEventHandler=this.handleEvent.bind(this),window.addEventListener("click",this.boundEventHandler)}teardown(){window.removeEventListener("click",this.boundEventHandler)}handleEvent(e){var t,r;if(!(!((t=this.routerSlot)===null||t===void 0)&&t.handleAnchorLinks))return;const n="composedPath"in e?e.composedPath().find(m=>m instanceof HTMLAnchorElement):e.target;if(n==null||!(n instanceof HTMLAnchorElement))return;const s=n.href.startsWith(location.origin),o=n.target!==""&&n.target!=="_self",l=n.dataset.routerSlot==="disabled",a=(r=this.routerSlot)===null||r===void 0?void 0:r.getRouteMatch(n.pathname),d=e.metaKey;if(!s||o||l||!a||d||e.metaKey)return;const u=`${n.pathname}${n.search}`;e.preventDefault(),history.pushState(null,"",u)}}const Fi=document.createElement("template");Fi.innerHTML="<slot></slot>";Hn();class ss extends HTMLElement{constructor(){super(),this.listeners=[],this._routes=[],this._handleAnchors=!0,this._routeMatch=null,this.render=this.render.bind(this),this.attachShadow({mode:"open"}).appendChild(Fi.content.cloneNode(!0))}get routes(){return this._routes}set routes(e){this.clear(),this.add(e)}get handleAnchorLinks(){return this._handleAnchors}set handleAnchorLinks(e){this._handleAnchors=e,e?this.setupAnchorListener():this.detachAnchorListener()}get parent(){return this._parent}set parent(e){this.detachListeners(),this._parent=e,this.attachListeners()}get isRoot(){return this.parent==null}get match(){return this._routeMatch}get route(){return this.match!=null?this.match.route:null}get fragments(){return this.match!=null?this.match.fragments:null}get params(){return this.match!=null?this.match.params:null}connectedCallback(){this.parent=this.queryParentRouterSlot(),this.setupAnchorListener()}disconnectedCallback(){this.detachListeners(),this.detachAnchorListener()}queryParentRouterSlot(){return Vi(this)}constructAbsolutePath(e){return zi(this,e)}add(e,t=this.isRoot&&this.isConnected){this._routes.push(...e),t&&this.render().then()}clear(){this._routes.length=0}getRouteMatch(e){return Zn(this._routes,e)}async render(){if(!this.isConnected)return;const e=this.parent!=null&&this.parent.fragments!=null?this.parent.fragments.rest:jn();await this.renderPath(e)}setupAnchorListener(){var e;this.isRoot&&(this.detachAnchorListener(),this.anchorHandler=new ns(this),(e=this.anchorHandler)===null||e===void 0||e.setup())}detachAnchorListener(){var e;(e=this.anchorHandler)===null||e===void 0||e.teardown()}attachListeners(){this.listeners.push(this.parent!=null?Q(this.parent,"changestate",this.render):Q(me,"changestate",this.render))}clearChildren(){for(;this.firstChild!=null;)this.firstChild.parentNode.removeChild(this.firstChild)}detachListeners(){Bi(this.listeners)}async renderPath(e){const t=this.getRouteMatch(e);if(t==null)return this._routeMatch=null,!1;const{route:r}=t,n={match:t,slot:this};try{const s=rs(this.match,t);if(s){let o=!1;const a=Q(me,"changestate",()=>o=!0,{once:!0}),d=()=>{a()},u=()=>(d(),z("navigationcancel",n),z("navigationend",n),!1);if(z("navigationstart",n),r.guards!=null){for(const m of r.guards)if(!await m(n))return u()}if(Xn(r))return d(),is(this,r),!1;if(Jn(r)){if(await r.resolve(n)===!1)return u()}else{const m=await Qn(r,n);if(o)return u();this.clearChildren(),this._routeMatch=t,this.appendChild(m)}d()}return this._routeMatch=t,requestAnimationFrame(()=>{zn(this,n)}),s&&(z("navigationsuccess",n),z("navigationend",n)),s}catch(s){throw z("navigationerror",n),z("navigationend",n),s}}}window.customElements.define(Ri,ss);const Wi=document.createElement("template");Wi.innerHTML="<slot></slot>";class os extends HTMLElement{constructor(){super(),this.listeners=[],this._context=null,this.navigate=this.navigate.bind(this),this.updateActive=this.updateActive.bind(this),this.attachShadow({mode:"open",delegatesFocus:this.delegateFocus}).appendChild(Wi.content.cloneNode(!0))}static get observedAttributes(){return["disabled"]}set path(e){this.setAttribute("path",e)}get path(){return this.getAttribute("path")||"/"}get target(){return this.getAttribute("target")}get disabled(){return this.hasAttribute("disabled")}set disabled(e){e?this.setAttribute("disabled",""):this.removeAttribute("disabled")}get active(){return this.hasAttribute("active")}set active(e){e?this.setAttribute("active",""):this.removeAttribute("active")}get delegateFocus(){return this.hasAttribute("delegateFocus")}set delegateFocus(e){e?this.setAttribute("delegateFocus",""):this.removeAttribute("delegateFocus")}get preserveQuery(){return this.hasAttribute("preservequery")}set preserveQuery(e){e?this.setAttribute("preservequery",""):this.removeAttribute("preservequery")}get context(){return this._context}set context(e){this._context=e}get absolutePath(){return this.constructAbsolutePath(this.path)}connectedCallback(){this.listeners.push(Q(this,"click",e=>this.navigate(this.path,e)),Q(this,"keydown",e=>e.code==="Enter"||e.code==="Space"?this.navigate(this.path,e):void 0),Q(me,"navigationend",this.updateActive),Q(me,"changestate",this.updateActive)),this.context=Vi(this),this.setAttribute("role","link"),this.hasAttribute("tabindex")||this.updateTabIndex()}disconnectedCallback(){Bi(this.listeners)}attributeChangedCallback(e,t,r){e==="disabled"&&this.updateTabIndex()}updateTabIndex(){this.tabIndex=this.disabled?-1:0}constructAbsolutePath(e){return this.context!=null?this.context.constructAbsolutePath(e):Ie(e,{end:!1})}updateActive(){const e=Gn(this.absolutePath);e!==this.active&&(this.active=e)}navigate(e,t){if(t!=null&&this.disabled){t.preventDefault(),t.stopPropagation();return}const r=this.preserveQuery?Ni():"",n=`${this.absolutePath}${r}`;if(this.target){window.open(n,this.target);return}if(t instanceof MouseEvent&&t.metaKey){window.open(n,"_blank");return}history.pushState(null,"",n)}}window.customElements.define("router-link",os);function as(i,e="long"){const t=ls(i),r=cs(i,t);return ds(r,t,e)}function ls(i){let e=1;return i>=1e12?e=1e12:i>=1e9?e=1e9:i>=1e6?e=1e6:i>=1e3&&(e=1e3),e}function cs(i=0,e){const t=i/e,r=t<10;let n=0;return r?n=Math.floor((t+Number.EPSILON)*10)/10:n=Math.floor(t),n}function ds(i,e,t){switch(e){case 1e12:return M(t==="short"?H`${i}T`:H`${i} trillion`);case 1e9:return M(t==="short"?H`${i}B`:H`${i} billion`);case 1e6:return M(t==="short"?H`${i}M`:H`${i} million`);case 1e3:return M(t==="short"?H`${i}K`:H`${i} thousand`);default:return`${i}`}}class hs{constructor(e){this.ownerId="donationBannerHandler",this.baseUrl=e.baseUrl}async pageNavigationOccurred(e=!0){if(window.location.pathname.startsWith("/donate")||!e){await this.removeDonationBanner();return}await this.addDonationBanner()}async removeDonationBanner(){var e;(e=this.bannerContainer)==null||e.remove()}async addDonationBanner(){if(this.bannerContainer)return;const e=document.createElement("div");e.dataset.owner=this.ownerId;const t=document.createElement("div");t.id="donato";const r=document.createElement("script");r.type="text/javascript",r.src=`https://${this.baseUrl}/includes/donate.js`,e.appendChild(t),e.appendChild(r),document.body.prepend(e)}get bannerContainer(){return document.body.querySelector(`[data-owner=${this.ownerId}]`)}}class us{constructor(e){this.endpoint="/services/donations/banner.php",this.fetchHandler=e.fetchHandler}async getBannerConfig(e){const t=new URLSearchParams;e!=null&&e.ymd&&t.append("ymd",e.ymd),e!=null&&e.variant&&t.append("variant",e.variant),e!=null&&e.platform&&t.append("platform",e.platform),e!=null&&e.recacheDonationBanner&&t.append("recacheDonationBanner",e.recacheDonationBanner);let r=this.endpoint;const n=t.toString();return n.length>0&&(r=`${r}?${n}`),(await this.fetchHandler.fetchApiPathResponse(r,{includeCredentials:!0})).value}}/**
 * @license
 * Copyright 2017 Google LLC
 * SPDX-License-Identifier: BSD-3-Clause
 */const mo={ATTRIBUTE:1,CHILD:2,PROPERTY:3,BOOLEAN_ATTRIBUTE:4},ps=i=>(...e)=>({_$litDirective$:i,values:e});class ms{constructor(e){}get _$AU(){return this._$AM._$AU}_$AT(e,t,r){this._$Ct=e,this._$AM=t,this._$Ci=r}_$AS(e,t){return this.update(e,t)}update(e,t){return this.render(...t)}}const ui=new WeakSet;class vs extends ms{constructor(e){super(e)}update(e,[t,r]){return ui.has(e)||(t(),ui.add(e)),this.render(t,r)}render(e,t){return t()}}const ne=ps(vs);function gs(i){return new Promise(e=>setTimeout(e,i))}class fs{constructor(e){var t,r,n,s;this.ARCHIVE_ANALYTICS_VERSION=2,this.DEFAULT_SERVICE="ao_2",this.NO_SAMPLING_SERVICE="ao_no_sampling",this.DEFAULT_IMAGE_URL="https://athena.archive.org/0.gif",this.defaultService=(t=e==null?void 0:e.defaultService)!==null&&t!==void 0?t:this.DEFAULT_SERVICE,this.imageUrl=(r=e==null?void 0:e.imageUrl)!==null&&r!==void 0?r:this.DEFAULT_IMAGE_URL,this.imageContainer=(n=e==null?void 0:e.imageContainer)!==null&&n!==void 0?n:document.body,this.requireImagePing=(s=e==null?void 0:e.requireImagePing)!==null&&s!==void 0?s:!1}sendPing(e){const t=this.generateTrackingUrl(e).toString();if(this.requireImagePing){this.sendPingViaImage(t);return}const r=navigator.sendBeacon&&navigator.sendBeacon.bind(navigator);try{r(t)}catch{this.sendPingViaImage(t)}}sendEvent(e){const t=e.label&&e.label.trim().length>0?e.label:window.location.pathname,r={kind:"event",ec:e.category,ea:e.action,el:t,cache_bust:Math.random(),...e.eventConfiguration};this.sendPing(r)}sendEventNoSampling(e){const t=e.eventConfiguration||{};t.service=this.NO_SAMPLING_SERVICE;const r=e;r.eventConfiguration=t,this.sendEvent(r)}sendPingViaImage(e){const t=new Image(1,1);t.src=e,t.alt="",this.imageContainer.appendChild(t)}generateTrackingUrl(e){var t;const r=e??{};r.service=(t=r.service)!==null&&t!==void 0?t:this.defaultService;const n=new URL(this.imageUrl),s=Object.keys(r);return s.forEach(o=>{const l=r[o];n.searchParams.append(o,l)}),n.searchParams.append("version",`${this.ARCHIVE_ANALYTICS_VERSION}`),n.searchParams.append("count",`${s.length+2}`),n}}class ys{constructor(e){this.analyticsManager=e}trackIaxParameter(e){const r=new URL(e).searchParams.get("iax");if(!r)return;const n=r.split("|"),s=n.length>=1?n[1]:"",o=n.length>=2?n[2]:"";this.analyticsManager.sendEventNoSampling({category:n[0],action:s,label:o})}trackPageView(e){const t={};t.kind="pageview",t.timediff=new Date().getTimezoneOffset()/60*-1,t.locale=navigator.language,t.referrer=document.referrer===""?"-":document.referrer;const{domInteractive:r,defaultFontSize:n}=this;r&&(t.loadtime=r),n&&(t.iaprop_fontSize=n),"devicePixelRatio"in window&&(t.iaprop_devicePixelRatio=window.devicePixelRatio),e!=null&&e.mediaType&&(t.iaprop_mediaType=e.mediaType),e!=null&&e.mediaLanguage&&(t.iaprop_mediaLanguage=e.mediaLanguage),e!=null&&e.primaryCollection&&(t.iaprop_primaryCollection=e.primaryCollection),e!=null&&e.page&&(t.page=e.page),this.analyticsManager.sendPing(t)}get defaultFontSize(){const e=window.getComputedStyle(document.documentElement);if(!e)return null;const t=e.fontSize,r=parseFloat(t)*1.6,n=t.replace(/(\d*\.\d+)|\d+/,"");return`${r}${n}`}get domInteractive(){if(!window.performance||!window.performance.getEntriesByType)return;const e=window.performance.getEntriesByType("navigation");return e.length===0?void 0:e[0].domInteractive}}class bs{constructor(e){e.enableAnalytics&&(this.analyticsBackend=new fs,this.analyticsHelpers=new ys(this.analyticsBackend))}sendPing(e){var t;(t=this.analyticsBackend)===null||t===void 0||t.sendPing(e)}sendEvent(e){var t;(t=this.analyticsBackend)===null||t===void 0||t.sendEvent(e)}send_event(e,t,r,n){this.sendEvent({category:e,action:t,label:r,eventConfiguration:n})}sendEventNoSampling(e){var t;(t=this.analyticsBackend)===null||t===void 0||t.sendEventNoSampling(e)}trackIaxParameter(e){var t;(t=this.analyticsHelpers)===null||t===void 0||t.trackIaxParameter(e)}trackPageView(e){var t;(t=this.analyticsHelpers)===null||t===void 0||t.trackPageView(e)}}class ws extends Error{constructor(e){super(e),this.name="ImportRetryError"}}const pi=new bs({enableAnalytics:!0}),ht=2;async function w(i,e=ht,t=1e3){try{return await i()}catch(r){let n="Error loading import";if(typeof r=="string"?n=r:r instanceof Error&&(n=r.message),e){await gs(t);const s=ht-e+1;return pi.sendEvent({category:"offshootImportRetry",action:"retryingImport",label:`retryNumber: ${s} / ${ht}, error: ${n}`}),w(i,e-1,t)}else throw pi.sendEvent({category:"offshootImportRetry",action:"importFailed",label:n}),new ws(n)}}async function $s(i,e){const n=(await(await(await i.get()).userService.get()).getLoggedInUser()).success;if(n)if(e==="settings")history.replaceState(history.state,"","/account/settings");else{const s=n.itemname,o=e?`/${e}`:"",l=window.location.search,a=window.location.hash;history.replaceState(history.state,"",`/details/${s}${o}${l}${a}`)}else window.location.pathname="/login"}/*! typescript-cookie v1.0.6 | MIT */const ji=i=>encodeURIComponent(i).replace(/%(2[346B]|5E|60|7C)/g,decodeURIComponent).replace(/[()]/g,escape),qi=i=>encodeURIComponent(i).replace(/%(2[346BF]|3[AC-F]|40|5[BDE]|60|7[BCD])/g,decodeURIComponent),Pt=decodeURIComponent,Mt=i=>(i[0]==='"'&&(i=i.slice(1,-1)),i.replace(/(%[\dA-F]{2})+/gi,decodeURIComponent));function _s(i){return i=Object.assign({},i),typeof i.expires=="number"&&(i.expires=new Date(Date.now()+i.expires*864e5)),i.expires!=null&&(i.expires=i.expires.toUTCString()),Object.entries(i).filter(([e,t])=>t!=null&&t!==!1).map(([e,t])=>t===!0?`; ${e}`:`; ${e}=${t.split(";")[0]}`).join("")}function Ki(i,e,t){const r=/(?:^|; )([^=]*)=([^;]*)/g,n={};let s;for(;(s=r.exec(document.cookie))!=null;)try{const o=t(s[1]);if(n[o]=e(s[2],o),i===o)break}catch{}return i!=null?n[i]:n}const mi=Object.freeze({decodeName:Pt,decodeValue:Mt,encodeName:ji,encodeValue:qi}),Rt=Object.freeze({path:"/"});function Gi(i,e,t=Rt,{encodeValue:r=qi,encodeName:n=ji}={}){return document.cookie=`${n(i)}=${r(e,i)}${_s(t)}`}function Yi(i,{decodeValue:e=Mt,decodeName:t=Pt}={}){return Ki(i,e,t)}function Ss({decodeValue:i=Mt,decodeName:e=Pt}={}){return Ki(void 0,i,e)}function Es(i,e=Rt){Gi(i,"",Object.assign({},e,{expires:-1}))}function _t(i,e){const t={set:function(n,s,o){return Gi(n,s,Object.assign({},this.attributes,o),{encodeValue:this.converter.write})},get:function(n){if(arguments.length===0)return Ss(this.converter.read);if(n!=null)return Yi(n,this.converter.read)},remove:function(n,s){Es(n,Object.assign({},this.attributes,s))},withAttributes:function(n){return _t(this.converter,Object.assign({},this.attributes,n))},withConverter:function(n){return _t(Object.assign({},this.converter,n),this.attributes)}},r={attributes:{value:Object.freeze(e)},converter:{value:Object.freeze(i)}};return Object.create(t,r)}_t({read:mi.decodeValue,write:mi.encodeValue},Rt);function As(i){return!(!i.includes("archive.org")||/login|signup|verify|logout|forgot-password|reset-password/.test(i))}class ks{constructor(e){this.accountRouteResolver=()=>($s(this.appServices),!1),this.authenticationGuard=async()=>await this.getLoggedInUser()?!0:(window.location.replace(`/login?referer=${encodeURIComponent(window.location.pathname+window.location.search)}`),!1),this.alreadyLoggedInGuard=async()=>{if(!await this.getLoggedInUser())return!0;const r=new URLSearchParams(window.location.search).get("referer");return!r||!As(r)?(window.location.replace("/"),!1):(window.location.replace(r),!1)},this.appServices=e}async makeAppRoutes(){const e=this.commonRoutes,t=this.archiveOrgUserRoutes;return this.guardRoutes(t,this.isArchiveOrgUser.bind(this)),e.push(...t),this.guardRoutes(e,this.isBetaAccessTokenExempt.bind(this)),e}guardRoutes(e,t){e.map(r=>{const n=r.guards||[];n.push(t),r.guards=n})}get commonRoutes(){return[{path:"donate",pathMatch:"full",component:()=>w(()=>f(()=>import("./donate-page-DRqrw3i6.js").then(t=>t.d),__vite__mapDeps([0,1,2,3,4,5,6]))),setup:t=>{const r=t,n=document.createElement("slot");n.name="donation-form-slot",n.slot="donation-form-slot",setTimeout(()=>{r.appendChild(n),r.appServices=this.appServices},0)}},{path:"/details/:id",component:()=>f(()=>import("./details-page-router-BNrFkR3B.js"),__vite__mapDeps([7,8])),setup:(t,r)=>{const n=decodeURIComponent(r.match.params.id),s=t;s.appServices=this.appServices,s.identifier=n}},{path:"/collection/:id",resolve:t=>{const r=t.match.params.id;return history.replaceState(history.state,"",`/details/${r}${window.location.search}`),!1}},{path:"/account/settings",component:()=>w(()=>f(()=>import("./account-settings-pYmZ8_sV.js"),__vite__mapDeps([9,10,11,12,13,14,8,15,16,1,17,4,5,18,19,2,20,21,22,23,24,6,25]))),setup:t=>{const r=t;r.appServices=this.appServices;const n=document.createElement("slot");n.name="center-of-form",n.slot="center-of-form",n.style.display="block",n.style.width="100%",setTimeout(()=>{r.appendChild(n),r.appServices=this.appServices},0)},guards:[this.authenticationGuard]},{path:"/account/$",resolve:this.accountRouteResolver},{path:"/search",component:()=>w(()=>f(()=>import("./search-page-D1iE3zZz.js"),__vite__mapDeps([26,27,17,28,29,30,31,32,33,13,34,5,35,25,36,10,16,19,4,37,38,39,40,41,42,6,24,43,44,45,46]))),setup:t=>{const r=t;r.appServices=this.appServices,r.smartCarouselType="single-tabbed",r.showMediatypeTabs=!0}},{path:"/$",component:()=>w(()=>f(()=>import("./home-page-D50WaHLp.js"),__vite__mapDeps([47,37,29,13,48,34,27,5,49,50,42,30,31,32,51,17]))),setup:t=>{const r=t;r.appServices=this.appServices}},{path:"/login",component:()=>f(()=>import("./login-page-BDbdnlJv.js"),__vite__mapDeps([52,8,17,12,15,16,13,14,11,10,1,4,5,18,19])),setup:t=>{const r=t,n=new URLSearchParams(window.location.search);r.appServices=this.appServices,r.referer=n.get("referer")??void 0,r.prefilledUsername=n.get("username")??void 0},guards:[this.alreadyLoggedInGuard]},{path:"/forgot-password",component:()=>w(()=>f(()=>import("./forgot-password-PqH6jkkY.js"),__vite__mapDeps([53,8,16,13]))),setup:t=>{const r=t;r.appServices=this.appServices}},{path:"/subscribe",component:()=>w(()=>f(()=>import("./subscribe-form-CZH5M5Cd.js"),__vite__mapDeps([54,6,25,15,16,13]))),setup:t=>{const r=t;r.appServices=this.appServices,r.baseHost="";const n=new URLSearchParams(window.location.search);r.email=n.get("email")??void 0,r.prefilledError=n.get("msg")??void 0,r.prefilledValues=n.get("mailing_lists_checkboxes")??void 0}},{path:"/about/:page",component:()=>w(()=>f(()=>import("./static-page-BN1ilDG4.js"),__vite__mapDeps([55,25,39,56,6,38,18,19]))),setup:(t,r)=>{const n=t,s=decodeURIComponent(r.match.params.page);s&&(n.page=s),n.appServices=this.appServices}},{path:"/about/$",component:()=>w(()=>f(()=>import("./static-page-BN1ilDG4.js"),__vite__mapDeps([55,25,39,56,6,38,18,19]))),setup:t=>{const r=t;r.appServices=this.appServices,r.page="about"}},{path:"/bookserver",component:()=>w(()=>f(()=>import("./static-page-BN1ilDG4.js"),__vite__mapDeps([55,25,39,56,6,38,18,19]))),setup:t=>{const r=t;r.appServices=this.appServices,r.page="bookserver"}},{path:"/legal/affidavit",component:()=>w(()=>f(()=>import("./static-page-BN1ilDG4.js"),__vite__mapDeps([55,25,39,56,6,38,18,19]))),setup:t=>{const r=t;r.page="legal-affidavit",r.appServices=this.appServices}},{path:"/legal/faq",component:()=>w(()=>f(()=>import("./static-page-BN1ilDG4.js"),__vite__mapDeps([55,25,39,56,6,38,18,19]))),setup:t=>{const r=t;r.page="legal-faq",r.appServices=this.appServices}},{path:"/legal/$",component:()=>w(()=>f(()=>import("./static-page-BN1ilDG4.js"),__vite__mapDeps([55,25,39,56,6,38,18,19]))),setup:t=>{const r=t;r.page="legal",r.appServices=this.appServices}},{path:"/projects",component:()=>w(()=>f(()=>import("./static-page-BN1ilDG4.js"),__vite__mapDeps([55,25,39,56,6,38,18,19]))),setup:t=>{const r=t;r.page="projects",r.appServices=this.appServices}},{path:"/scanning",component:()=>w(()=>f(()=>import("./static-page-BN1ilDG4.js"),__vite__mapDeps([55,25,39,56,6,38,18,19]))),setup:t=>{const r=t;r.page="scanning",r.appServices=this.appServices}},{path:"/web/petabox",component:()=>w(()=>f(()=>import("./static-page-BN1ilDG4.js"),__vite__mapDeps([55,25,39,56,6,38,18,19]))),setup:t=>{const r=t;r.page="petabox",r.appServices=this.appServices}},{path:"/web/sflan",component:()=>w(()=>f(()=>import("./static-page-BN1ilDG4.js"),__vite__mapDeps([55,25,39,56,6,38,18,19]))),setup:t=>{const r=t;r.page="sflan",r.appServices=this.appServices}},{path:"/labs/search/smart-row",component:()=>w(()=>f(()=>import("./search-page-D1iE3zZz.js"),__vite__mapDeps([26,27,17,28,29,30,31,32,33,13,34,5,35,25,36,10,16,19,4,37,38,39,40,41,42,6,24,43,44,45,46]))),setup:t=>{const r=t;r.appServices=this.appServices,r.smartCarouselType="single"}},{path:"/labs/search/smart-carousels",component:()=>w(()=>f(()=>import("./search-page-D1iE3zZz.js"),__vite__mapDeps([26,27,17,28,29,30,31,32,33,13,34,5,35,25,36,10,16,19,4,37,38,39,40,41,42,6,24,43,44,45,46]))),setup:t=>{const r=t;r.appServices=this.appServices,r.smartCarouselType="multi"}},{path:"/labs/search/smart-bubbles",component:()=>w(()=>f(()=>import("./search-page-D1iE3zZz.js"),__vite__mapDeps([26,27,17,28,29,30,31,32,33,13,34,5,35,25,36,10,16,19,4,37,38,39,40,41,42,6,24,43,44,45,46]))),setup:t=>{const r=t;r.appServices=this.appServices,r.showSmartFacetBar=!0}},{path:"/labs/search-discovery",component:()=>w(()=>f(()=>import("./discover-page-DlB9tJgE.js"),__vite__mapDeps([57,34,27,5,17,33,13,30,31,32,35,25,36,10,16,19,4,29,37,38,39,40,42]))),setup:t=>{const r=t;r.appServices=this.appServices}},{path:"/labs/search",redirectTo:"/search",preserveQuery:!0}]}get archiveOrgUserRoutes(){return[{path:"/labs/details/:id",component:()=>f(()=>import("./details-page-BGRQ2-s1.js"),__vite__mapDeps([58,6,25,4,5,31,32,59,60,27,28,30,49,50,17,36,10,16,13,19,22,61,29,39,62,63])),setup:(t,r)=>{const n=t,s=decodeURIComponent(r.match.params.id);n.appServices=this.appServices,n.identifier=s}}]}async getLoggedInUser(){return(await(await(await this.appServices.get()).userService.get()).getLoggedInUser()).success}async isArchiveOrgUser(){const e=await this.getLoggedInUser();return!!(e!=null&&e.isArchiveOrgUser)}async isBetaAccessTokenExempt(){const t=await(await this.appServices.get()).appConfig.get(),{environment:r,archiveApiUrl:n}=t;if(r==="prod"||n==="archive.org"||!await this.isArchiveOrgUser())return!0;const l=Yi("beta-access")===void 0;if(l){const a=document.createElement("div");a.id="beta-access-message",a.innerHTML=`
        Sending you to the <a href="https://archive.org/services/beta-access">beta access page</a>
      `,setTimeout(()=>{window.location.href="https://archive.org/services/beta-access"},3e3),document.body.appendChild(a)}return!l}}var Is=Object.defineProperty,Cs=Object.getOwnPropertyDescriptor,Fe=(i,e,t,r)=>{for(var n=r>1?void 0:r?Cs(e,t):e,s=i.length-1,o;s>=0;s--)(o=i[s])&&(n=(r?o(e,t,n):o(n))||n);return r&&n&&Is(e,t,n),n};let Ae=class extends P{constructor(){super(...arguments),this.adjustSpeedForWidth=!0}render(){return p`
      <div class="track">
        <div class="bar"></div>
      </div>
    `}updated(i){const e=i.get("resizeObserver");e&&e.removeObserver({handler:this,target:this.track}),(i.has("adjustSpeedForWidth")||i.has("resizeObserver"))&&(this.adjustSpeedForWidth?this.setupResizeObserver():this.disconnectResizeObserver())}handleResize(i){const e=i.contentRect.width;switch(i.target){case this.track:this.updateLoaderSpeed(e);break}}disconnectedCallback(){this.disconnectResizeObserver()}updateLoaderSpeed(i){var t,r;const e=Math.max(i/200,3);(r=(t=this.shadowRoot)==null?void 0:t.host)==null||r.style.setProperty("--horizontalActivityIndicatorDuration",e+"s")}disconnectResizeObserver(){var i;(i=this.resizeObserver)==null||i.removeObserver({handler:this,target:this.track})}async setupResizeObserver(){var i;(i=this.resizeObserver)==null||i.addObserver({handler:this,target:this.track})}static get styles(){const i=g`var(--horizontalActivityIndicatorColor, dodgerblue)`,e=g`var(--horizontalActivityIndicatorDuration, 7.5s)`;return g`
      :host {
        display: block;
      }

      @keyframes loader-animation {
        0% {
          left: -100%;
          width: 100%;
        }
        49% {
          left: 100%;
          width: 10%;
        }
        50% {
          left: 100%;
          width: 100%;
        }
        100% {
          left: -10%;
          width: 10%;
        }
      }

      .track {
        height: 100%;
        width: 100%;
        overflow: hidden;
      }

      .track .bar {
        position: relative;
        height: 100%;
        background-color: ${i};
        animation-name: loader-animation;
        animation-duration: ${e};
        animation-iteration-count: infinite;
        animation-timing-function: ease-in-out;
      }
    `}};Fe([h({type:Object})],Ae.prototype,"resizeObserver",2);Fe([h({type:Boolean})],Ae.prototype,"adjustSpeedForWidth",2);Fe([re(".track")],Ae.prototype,"track",2);Ae=Fe([A("horizontal-activity-indicator")],Ae);const xs={VITE_PUBLIC_ACCOUNT_SETTINGS_SERVICE_URL:"https://archive.org/services/account/settings/",VITE_PUBLIC_ARCHIVE_API_URL:"archive.org",VITE_PUBLIC_ARCHIVE_FQDN_API_URL:"https://archive.org",VITE_PUBLIC_ARCHIVE_IMAGE_URL:"https://archive.org",VITE_PUBLIC_ARCHIVE_IT_COLLECTION_URL:"https://archive-it.org/collections/",VITE_PUBLIC_ARCHIVE_IT_PARTNER_URL:"https://archive-it.org/organizations/",VITE_PUBLIC_ARCHIVE_NAV_URL:"",VITE_PUBLIC_BASE_PATH:"/offshoot_assets/",VITE_PUBLIC_BRAINTREE_AUTH_TOKEN:"production_x6ffdgk2_pqd7hz44swp6zvvw",VITE_PUBLIC_ENVIRONMENT:"prod",VITE_PUBLIC_FAVORITES_SERVICE_URL:"https://archive.org/services/offshoot/details-page/favorite.php",VITE_PUBLIC_FEATURE_FEEDBACK_SERVICE_URL:"https://archive.org/services/offshoot/feature-feedback.php",VITE_PUBLIC_GOOGLEPAY_MERCHANT_ID:"14255500436693515703",VITE_PUBLIC_GOOGLE_CLIENT_ID:"41383750805-slra1gn7ge0bcc8ihpqnkk7hf0fo7dem.apps.googleusercontent.com",VITE_PUBLIC_ITEM_FLAG_SERVICE_URL:"https://archive.org/services/content-flags/",VITE_PUBLIC_ITEM_MANAGEMENT_URL:"https://archive.org/manage/",VITE_PUBLIC_RADIO_SEARCH_URL:"https://archive.org/search.php",VITE_PUBLIC_RECAPTCHA_SITE_KEY:"6Ld64a8UAAAAAGbDwi1927ztGNw7YABQ-dqzvTN2",VITE_PUBLIC_REVIEWS_MAX_BODY_CHARS:"1000",VITE_PUBLIC_REVIEWS_MAX_TITLE_CHARS:"100",VITE_PUBLIC_SENTRY_DSN:"https://f36cc7e0e3484c2ab9aa70f5999ee9ca@sink.archive.org/2",VITE_PUBLIC_SENTRY_ERROR_SAMPLE_RATE:"0.01",VITE_PUBLIC_SENTRY_SDK_DEBUG_LOGGING:"false",VITE_PUBLIC_SENTRY_TRACES_SAMPLE_RATE:"0.001",VITE_PUBLIC_SUPPRESS_FACETS:"false",VITE_PUBLIC_TESTING:"false",VITE_PUBLIC_TV_SEARCH_URL:"/tv",VITE_PUBLIC_USER_LISTS_API_URL:"https://archive.org",VITE_PUBLIC_VENMO_PROFILE_ID:"2878003111190856236",VITE_PUBLIC_VERSION:"main---340e5d79",VITE_PUBLIC_WAYBACK_SEARCH_URL:"https://web.archive.org/web/*/",VITE_PUBLIC_WAYBACK_URL:"web.archive.org",VITE_PUBLIC_WEB_ARCHIVES_SERVICE_URL:"https://archive.org/services/web-archives/service.php",VITE_PUBLIC_ZENDESK_HELP_WIDGET_KEY:"685f6dc4-48c5-411f-8463-cc6dd50abe2d"};var Ls=Object.defineProperty,Ts=Object.getOwnPropertyDescriptor,_=(i,e,t,r)=>{for(var n=r>1?void 0:r?Ts(e,t):e,s=i.length-1,o;s>=0;s--)(o=i[s])&&(n=(r?o(e,t,n):o(n))||n);return r&&n&&Ls(e,t,n),n};let $=class extends P{constructor(){super(...arguments),this.appServices=new ct({generator:async()=>(await w(()=>f(()=>import("./app-services-CYrX05nP.js"),[]))).AppServices.generateAppServices({appRoot:this,appConfig:this.appConfig,modalManager:new ct({generator:async()=>this.modalManager})})}),this.appConfig=new ct({generator:async()=>{const i=xs,r=new URLSearchParams(window.location.search).get("prerender")==="1",n=i.VITE_PUBLIC_TESTING==="true",s=i.VITE_PUBLIC_SUPPRESS_FACETS==="true",o=i.VITE_PUBLIC_SENTRY_SDK_DEBUG_LOGGING==="true",l=parseFloat(i.VITE_PUBLIC_SENTRY_TRACES_SAMPLE_RATE),a=parseFloat(i.VITE_PUBLIC_SENTRY_ERROR_SAMPLE_RATE);return{environment:i.VITE_PUBLIC_ENVIRONMENT,documentReferrer:document.referrer,basePath:i.VITE_PUBLIC_BASE_PATH,version:i.VITE_PUBLIC_VERSION,archiveNavUrl:i.VITE_PUBLIC_ARCHIVE_NAV_URL,archiveApiUrl:i.VITE_PUBLIC_ARCHIVE_API_URL,archiveFQDNApiUrl:i.VITE_PUBLIC_ARCHIVE_FQDN_API_URL,itemFlagServiceUrl:i.VITE_PUBLIC_ITEM_FLAG_SERVICE_URL,waybackUrl:i.VITE_PUBLIC_WAYBACK_URL,waybackSearchUrl:i.VITE_PUBLIC_WAYBACK_SEARCH_URL,tvSearchUrl:i.VITE_PUBLIC_TV_SEARCH_URL,radioSearchUrl:i.VITE_PUBLIC_RADIO_SEARCH_URL,archiveItCollectionSearchUrl:i.VITE_PUBLIC_ARCHIVE_IT_COLLECTION_URL,archiveItPartnerSearchUrl:i.VITE_PUBLIC_ARCHIVE_IT_PARTNER_URL,userListsUrl:i.VITE_PUBLIC_USER_LISTS_API_URL,itemManagementUrl:i.VITE_PUBLIC_ITEM_MANAGEMENT_URL,webArchivesServiceUrl:i.VITE_PUBLIC_WEB_ARCHIVES_SERVICE_URL,favoriteServiceUrl:i.VITE_PUBLIC_FAVORITES_SERVICE_URL,accountSettingsServiceUrl:i.VITE_PUBLIC_ACCOUNT_SETTINGS_SERVICE_URL,sentry:{dsn:i.VITE_PUBLIC_SENTRY_DSN,tracesSampleRate:l,sampleRate:a,debugLogging:o},zendeskHelpWidgetKey:i.VITE_PUBLIC_ZENDESK_HELP_WIDGET_KEY,isPrerendering:r,isTesting:n,suppressFacets:s,recaptchaSiteKey:i.VITE_PUBLIC_RECAPTCHA_SITE_KEY,donationForm:{braintreeAuthToken:i.VITE_PUBLIC_BRAINTREE_AUTH_TOKEN,venmoProfileId:i.VITE_PUBLIC_VENMO_PROFILE_ID,googlepayMerchantId:i.VITE_PUBLIC_GOOGLEPAY_MERCHANT_ID},featureFeedback:{serviceUrl:i.VITE_PUBLIC_FEATURE_FEEDBACK_SERVICE_URL},baseImageUrl:i.VITE_PUBLIC_ARCHIVE_IMAGE_URL,currentBaseUrl:window.location.origin,googleClientId:i.VITE_PUBLIC_GOOGLE_CLIENT_ID,reviews:{maxTitleChars:parseInt(i.VITE_PUBLIC_REVIEWS_MAX_TITLE_CHARS),maxBodyChars:parseInt(i.VITE_PUBLIC_REVIEWS_MAX_BODY_CHARS)}}}}),this.footerPosition="off",this.hasLoadedStrings=!1,this.showDebugBorders=!1,this.showDevMenu=!1,this.activityIndicatorVisible=!0,this.siteHasLimitedFunctionality=!1,this.waybackPagesArchived="",this.screenName="",this.username="",this.userHasItemsPriv=!1,this.userHasFlagsPriv=!1,this.zendeskWidgetVisible=!1,this.zendeskWidgetKey="",this.iaxParameterTracked=!1,this.activityIndicatorRequests=0}firstUpdated(){this.startup()}updated(i){var e;i.has("appServices")&&this.appServices&&this.setupAppServicesDependents(),i.has("hasLoadedStrings")&&this.hasLoadedStrings&&this.setupView(),i.has("donationBannerVisible")&&this.donationBannerVisible&&((e=this.donationBannerHandler)==null||e.pageNavigationOccurred(!!this.donationBannerVisible))}async startup(){this.checkForLimitedFunctionality(),await Promise.allSettled([this.setupDonationBannerService(),this.setupDonationBannerHandler(),this.setupPsaBanner(),this.setupKeyValueStoreManager(),this.setupZendeskWidget(),this.startupSentry()])}async setupPolyfills(){await Promise.allSettled([this.setupIntersectionObserverPolyfill(),this.setupPopoverPolyfill(),this.setupElementInternalsPolyfill()])}async setupIntersectionObserverPolyfill(){var i,e;if(!("IntersectionObserver"in window))try{const t=await w(()=>f(()=>import("./index-X5ki3g7A.js"),[]));window.IntersectionObserver=t.default,this.trackPolyfillEvent("loadIntersectionObserver")}catch(t){this.trackPolyfillEvent("failedLoadIntersectionObserver"),(e=(i=window.Sentry)==null?void 0:i.captureMessage)==null||e.call(i,"Failed to load IntersectionObserver polyfill: "+t,"error")}}async setupPopoverPolyfill(){var i,e;if(!("popover"in HTMLElement.prototype))try{await w(()=>f(()=>import("./popover-c_mFcprL.js"),[])),this.trackPolyfillEvent("loadPopovers")}catch(t){this.trackPolyfillEvent("failedLoadPopovers"),(e=(i=window.Sentry)==null?void 0:i.captureMessage)==null||e.call(i,"Failed to load Popover polyfill: "+t,"error")}}async setupElementInternalsPolyfill(){var i,e;if(!("attachInternals"in HTMLElement.prototype))try{await w(()=>f(()=>import("./index-BDQVb_IC.js"),[])),this.trackPolyfillEvent("loadElementInternals")}catch(t){this.trackPolyfillEvent("failedLoadElementInternals"),(e=(i=window.Sentry)==null?void 0:i.captureMessage)==null||e.call(i,"Failed to load Element Internals polyfill: "+t,"error")}}async setupPsaBanner(){const i=await this.appServices.get();await Promise.all([this.psaBannerService=await i.psaBannerService.get(),this.userService=await i.userService.get(),this.localCache=await i.localCache.get()])}async setupKeyValueStoreManager(){const i=await this.appServices.get();this.keyValueStoreManager=await i.keyValueStoreManager.get()}async setupZendeskWidget(){const i=await this.appServices.get(),e=await i.appConfig.get(),r=await(await i.configService.get()).getConfig("feature.zendesk_enabled");this.zendeskWidgetKey=e.zendeskHelpWidgetKey,this.zendeskWidgetVisible=r!==""}async setupAppServicesDependents(){await Promise.allSettled([this.setupDevMenu(),this.setupLocalizationManager(),this.startupThemeManager(),this.trackIaxParameter(),this.setupPageViewAnalytics(),this.setupPageMetadataService(),this.setupLightDOMTunnelReset(),this.setupResizeObserver()])}async startupSentry(){const i=await this.appConfig.get();if(i.isTesting||i.isPrerendering)return;const{SentryConfigurer:e}=await w(()=>f(()=>import("./sentry-AjyKTUQ5.js"),__vite__mapDeps([64,23])));new e(i).configure()}async checkForLimitedFunctionality(){const e=new URLSearchParams(window.location.search).get("limited");this.siteHasLimitedFunctionality=e==="1"}async trackIaxParameter(){if(this.iaxParameterTracked)return;(await(await this.appServices.get()).analyticsHandler.get()).trackIaxParameter(window.location.href),this.iaxParameterTracked=!0}async setupDevMenu(){const e=await(await this.appServices.get()).appConfig.get(),t=await this.getLoggedInUser(),r=e.environment==="dev",n=t&&t.privs.includes("offshoot-dev")||!1;this.showDevMenu=r||n}async setupPageViewAnalytics(){window.addEventListener("navigationend",async()=>{(await(await this.appServices.get()).analyticsHandler.get()).trackPageView()})}async setupPageMetadataService(){window.addEventListener("navigationstart",async()=>{const e=await(await this.appServices.get()).pageMetadataService.get();e.clearMetaTags(),e.clearLinkTags()})}async setupLightDOMTunnelReset(){window.addEventListener("navigationstart",async()=>{(await(await this.appServices.get()).lightDOMTunnel.get()).removeAll()})}async setupResizeObserver(){const e=await(await this.appServices.get()).resizeObserver.get();this.sharedResizeObserver=e}async setupPageNavigationActivityIndicator(){window.addEventListener("navigationstart",async()=>{this.showActivityIndicator(!0)}),window.addEventListener("navigationend",async()=>{this.showActivityIndicator(!1)})}async setupDonationBannerService(){const e=await(await this.appServices.get()).fetchHandler.get();this.donationBannerService=new us({fetchHandler:e});const t=new URLSearchParams(window.location.search),r=t.get("ymd")??void 0,n=t.get("variant")??void 0,s=t.get("platform")??void 0,o=t.get("recacheDonationBanner")??void 0,l=await this.donationBannerService.getBannerConfig({ymd:r,variant:n,platform:s,recacheDonationBanner:o});l&&(this.donationBannerVisible=l.shouldDisplayBanner)}async setupDonationBannerHandler(){const i=await this.appConfig.get();this.donationBannerHandler=new hs({baseUrl:i.archiveApiUrl}),window.addEventListener("navigationend",async()=>{var e;await((e=this.donationBannerHandler)==null?void 0:e.pageNavigationOccurred(!!this.donationBannerVisible))})}async setupView(){await this.setupPolyfills(),await Promise.allSettled([this.setupPageNavigationActivityIndicator(),this.setupRouter(),this.setupTopnav()])}async setFooterPosition(i){this.footerPosition=i}showTopNavSearch(i){this.topNav.hideSearch=!i}showTopNav2ndLogo(i){const e=i?"allow":"";this.topNav.secondIdentitySlotMode=e}showZendeskWidget(i){this.zendeskWidgetVisible=i}setItemIdentifier(i){this.itemIdentifier=i}addAllowedCustomElementsToPage(){this.convertDayInHistoryLinks(),this.convertDisabilityEligibilityForms()}convertDayInHistoryLinks(){var t,r;const i=(t=this.routerSlot)==null?void 0:t.querySelector("collection-page"),e=(r=i==null?void 0:i.shadowRoot)==null?void 0:r.querySelectorAll('a[onclick*="tapeoftheday("]');e==null||e.forEach(n=>{const s=n.textContent,o=document.createElement("day-in-history");o.textContent=s,n.replaceWith(o)})}convertDisabilityEligibilityForms(){var r,n,s;const i=(r=this.routerSlot)==null?void 0:r.querySelector("collection-page"),e=(n=i==null?void 0:i.shadowRoot)==null?void 0:n.querySelector("collection-about"),t=(s=e==null?void 0:e.shadowRoot)==null?void 0:s.querySelectorAll(".disability-eligibility-provider-form");t==null||t.forEach(o=>{var v,b,x;const l=o.parentElement,a=document.createElement("disability-eligibility-form"),d=o.querySelector("form");a.setAttribute("type",(d==null?void 0:d.dataset.disabilityEligibilityType)??""),a.toggleAttribute("ispassword",!!o.querySelector('input[type="password"]')),a.toggleAttribute("open",(l==null?void 0:l.open)??!1);const u=l==null?void 0:l.querySelector("h2");u==null||u.setAttribute("slot","title"),a.append(u??"");const m=document.createElement("span");m.textContent=((v=o.querySelector("label"))==null?void 0:v.textContent)??"",m.setAttribute("slot","input-label"),a.append(m),(b=o.querySelectorAll("p"))==null||b.forEach(C=>{a.append(C)}),(x=o.parentElement)==null||x.replaceWith(a)})}getWindowScrollbarWidth(){return window.innerWidth-document.documentElement.clientWidth}showActivityIndicator(i){this.activityIndicatorRequests+=i?1:-1,this.activityIndicatorRequests=Math.max(0,this.activityIndicatorRequests);const e=!i&&this.activityIndicatorRequests===0;this.activityIndicatorVisible=!e}async isListsCalloutEnabled(){var i,e;try{const t=await this.appServices.get(),[r,n]=await Promise.all([t.keyValueStoreManager.get(),t.abtestManager.get()]),s=r==null?void 0:r.getKeyValueStore({namespace:"UserLists",storageType:"localStorage"}),[o,l]=await Promise.all([n.variantFor("UserListsLaunchCallouts"),s==null?void 0:s.get("tabCalloutDismissed")]);return o==="On"&&!l}catch(t){return(e=(i=window.Sentry)==null?void 0:i.captureMessage)==null||e.call(i,"Unable to load lists callout state: "+t,"error"),!1}}async setupTopnav(){await Promise.allSettled([this.updateTopNavConfig(),this.updateTopNavUserInfo(),this.updateTopNavCallouts(),this.updateWaybackPagesArchived()])}async updateTopNavConfig(){const i=await this.appConfig.get();this.topNav.baseHost=i.archiveNavUrl}async updateTopNavUserInfo(){const i=await this.getLoggedInUser();if(!i)return;this.screenName=i.screenname,this.username=i.itemname.replace(/^@/,"");const e=i.privs.includes("/");this.userHasItemsPriv=e||i.privs.includes("/items"),this.userHasFlagsPriv=e||i.privs.includes("/flags")}async updateTopNavCallouts(){const i=await this.isListsCalloutEnabled();this.topNav.config&&(this.topNav.config={...this.topNav.config,callouts:i?{"My lists":"NEW"}:{}})}async updateWaybackPagesArchived(){const t=await(await(await this.appServices.get()).mediaCountService.get()).fetchMediaCounts(),r=as((t==null?void 0:t.web)??0);this.waybackPagesArchived=r}async getLoggedInUser(){return(await(await(await this.appServices.get()).userService.get()).getLoggedInUser()).success}async startupThemeManager(){await(await(await this.appServices.get()).themeManager.get()).startup()}async setupRouter(){if(!this.routerSlot)return;const i=new ks(this.appServices);this.routerSlot.handleAnchorLinks=!1;const e=await i.makeAppRoutes();this.routerSlot.add(e)}async setupLocalizationManager(){const e=await(await this.appServices.get()).localizationManager.get();this.showActivityIndicator(!0),await e.setup(),this.hasLoadedStrings=!0,this.showActivityIndicator(!1)}render(){return p`
      <a
        href="#maincontent"
        id="skip-to-main-content-link"
        @click=${this.skipToMainContent}
        >${M("Skip to main content")}</a
      >

      <header>
        ${this.psaBannerTemplate}
        ${this.siteHasLimitedFunctionality?this.limitedSiteFunctionalityBannerTemplate:y}
        <ia-topnav
          ?localLinks=${!0}
          .waybackPagesArchived=${this.waybackPagesArchived}
          @trackClick=${this.trackTopNavEvent}
          @trackSubmit=${this.trackTopNavEvent}
          screenName=${this.screenName}
          username=${this.username}
          itemIdentifier=${Ct(this.itemIdentifier)}
          ?admin=${this.userHasItemsPriv}
          ?canManageFlags=${this.userHasFlagsPriv}
        >
        </ia-topnav>
        ${this.showDevMenu?this.devMenuTemplate:y}
        ${this.activityIndicatorVisible?this.activityIndicatorTemplate:y}
      </header>

      ${this.hasLoadedStrings?this.mainTemplate:y}
      ${this.modalManagerTemplate}
      ${this.zendeskWidgetVisible?this.zendeskHelpWidgetTemplate:y}
    `}get psaBannerTemplate(){return ne(async()=>{await w(()=>f(()=>import("./psa-banners-CJ8F4s-y.js"),__vite__mapDeps([65,6,66])))},()=>p`
          <psa-banners
            .psaBannerService=${this.psaBannerService}
            .userService=${this.userService}
            .keyValueStoreManager=${this.keyValueStoreManager}
          >
          </psa-banners>
        `)}skipToMainContent(i){var e;i.preventDefault(),(e=this.mainContent)==null||e.scrollIntoView({behavior:"smooth"}),setTimeout(()=>{var t;(t=this.mainContent)==null||t.focus()},250)}get limitedSiteFunctionalityBannerTemplate(){return ne(async()=>{await w(()=>f(()=>import("./alert-banner-D5y_Mm5l.js"),[]))},()=>p`
          <alert-banner .level=${"warn"}>
            ${M("Your browser may not be compatible with all the features on this site. Consider upgrading to a modern browser for an improved experience.")}
          </alert-banner>
        `)}get zendeskHelpWidgetTemplate(){return ne(async()=>{await w(()=>f(()=>import("./index-jBrPYLBf.js"),__vite__mapDeps([67,1])))},()=>p`
          <ia-zendesk-help-widget
            widgetSrc="https://static.zdassets.com/ekr/snippet.js?key=${this.zendeskWidgetKey}"
          ></ia-zendesk-help-widget>
        `)}get devMenuTemplate(){return ne(async()=>{await w(()=>f(()=>import("./dev-tools-BHvjMMdg.js"),__vite__mapDeps([68,69,3,70])))},()=>p`
          <dev-tools
            .appServices=${this.appServices}
            @pageWidthSliderChanged=${this.devToolPageWidthSliderChanged}
            @borderCheckboxChecked=${this.devToolsBorderCheckboxChecked}
          >
          </dev-tools>
        `)}get activityIndicatorTemplate(){return p`
      <horizontal-activity-indicator
        .resizeObserver=${this.sharedResizeObserver}
      >
      </horizontal-activity-indicator>
    `}get mainTemplate(){return p`
      <main id="maincontent" tabindex="-1">
        <div
          class="page-container ${this.showDebugBorders?"show-debug-borders":""}"
        >
          <router-slot></router-slot>
        </div>
      </main>

      <footer class="${this.footerPosition}">${this.footerTemplate}</footer>
    `}get modalManagerTemplate(){return ne(async()=>{await w(()=>f(()=>import("./index-Bj--Zo_c.js"),__vite__mapDeps([4,5])))},()=>p`
          <modal-manager>
            <slot
              name="paypal-upsell-button"
              slot="paypal-upsell-button"
            ></slot>
          </modal-manager>
        `)}devToolPageWidthSliderChanged(i){var t,r;const e=i.detail.width;(r=(t=this.shadowRoot)==null?void 0:t.host)==null||r.style.setProperty("--app-root-page-max-width",e+"rem")}devToolsBorderCheckboxChecked(i){const e=i.detail.showBorders;this.showDebugBorders=e}async trackTopNavEvent(i){var r,n;const[e,t]=(n=(r=i.detail)==null?void 0:r.event)==null?void 0:n.split("|");!e||!t||await this.trackEvent({event:{category:e,action:t},sampling:!0})}async trackPolyfillEvent(i){await this.trackEvent({event:{category:"offshootPolyfills",action:i,label:navigator.userAgent},sampling:!0})}async trackEvent(i){const t=await(await this.appServices.get()).analyticsHandler.get();i.sampling?t.sendEvent(i.event):t.sendEventNoSampling(i.event)}get footerTemplate(){return ne(async()=>{await w(()=>f(()=>import("./app-footer-BYhV8erm.js"),[]))},()=>p` <app-footer></app-footer> `)}static get styles(){return g`
      #skip-to-main-content-link {
        /*http://webaim.org/techniques/css/invisiblecontent/*/
        position: absolute;
        clip: rect(0, 0, 0, 0);
        top: auto;
        width: 1px;
        height: 1px;
        overflow: hidden;
        font-size: 1.4rem;
        color: var(--ia-theme-link-color, #4b64ff);
      }
      #skip-to-main-content-link:focus {
        position: static;
        width: auto;
        height: auto;
        outline: thin dotted #333;
        outline: 3px auto -webkit-focus-ring-color;
        margin: 5px 0 2px 8px;
        padding: 2px 3px;
        display: inline-block;
      }
      #skip-to-main-content-link:visited {
        color: var(--ia-theme-link-color, #4b64ff);
      }

      horizontal-activity-indicator {
        position: absolute;
        height: 3px;
        width: 100%;
      }
      modal-manager[mode='closed'] {
        display: none;
      }
      modal-manager.more-search-facets {
        --modalWidth: 85rem;
        --modalBorder: 2px solid #194880;
        --modalTitleLineHeight: 4rem;
        --modalTitleFontSize: 1.8rem;
        --modalCornerRadius: 0;
        --modalBottomPadding: 0;
        --modalScrollOffset: 0;
        --modalCornerRadius: 0.5rem;
      }
      modal-manager.expanded-date-picker {
        --modalWidth: 58rem;
        --modalBorder: 2px solid var(--primaryButtonBGColor, #194880);
        --modalTitleLineHeight: 4rem;
        --modalTitleFontSize: 1.8rem;
        --modalCornerRadius: 0;
        --modalBottomPadding: 0;
        --modalBottomMargin: 0;
        --modalScrollOffset: 0;
        --modalCornerRadius: 0.5rem;
      }
      modal-manager.remove-items {
        --modalWidth: 58rem;
        --modalBorder: 2px solid var(--primaryButtonBGColor, #194880);
        --modalTitleLineHeight: 4rem;
        --modalTitleFontSize: 1.8rem;
        --modalCornerRadius: 0;
        --modalBottomPadding: 0;
        --modalBottomMargin: 0;
        --modalScrollOffset: 0;
        --modalCornerRadius: 0.5rem;
      }
      modal-manager.create-user-list,
      modal-manager.default {
        --modalTitleLineHeight: 4.5rem;
        --modalHeaderBottomPadding: 0;
        --modalWidth: 40rem;
      }
      modal-manager.delete-user-list {
        --modalTitleLineHeight: 4.5rem;
        --modalHeaderBottomPadding: 0;
        --modalWidth: 40rem;
      }
      modal-manager.share-modal-content {
        --modalWidth: 50rem;
        --modalBottomPadding: 1rem;
      }

      app-footer {
        display: block;
        margin-top: 1.5rem;
      }
      .page-container {
        margin: auto;
      }
      .page-container.show-debug-borders {
        outline: 1px solid purple;
      }
      footer {
        width: 100%;
        left: 0;
        right: 0;
        bottom: 0;
        z-index: 10;
      }
      footer.fixed {
        position: fixed;
      }
      footer.off {
        display: none;
      }
    `}};_([h({type:Object})],$.prototype,"appServices",2);_([h({type:Object})],$.prototype,"appConfig",2);_([S()],$.prototype,"footerPosition",2);_([S()],$.prototype,"hasLoadedStrings",2);_([S()],$.prototype,"showDebugBorders",2);_([S()],$.prototype,"showDevMenu",2);_([S()],$.prototype,"activityIndicatorVisible",2);_([S()],$.prototype,"sharedResizeObserver",2);_([S()],$.prototype,"siteHasLimitedFunctionality",2);_([S()],$.prototype,"waybackPagesArchived",2);_([S()],$.prototype,"screenName",2);_([S()],$.prototype,"username",2);_([S()],$.prototype,"userHasItemsPriv",2);_([S()],$.prototype,"userHasFlagsPriv",2);_([S()],$.prototype,"zendeskWidgetVisible",2);_([S()],$.prototype,"zendeskWidgetKey",2);_([S()],$.prototype,"donationBannerVisible",2);_([S()],$.prototype,"psaBannerService",2);_([S()],$.prototype,"userService",2);_([S()],$.prototype,"localCache",2);_([S()],$.prototype,"keyValueStoreManager",2);_([S()],$.prototype,"itemIdentifier",2);_([re("router-slot")],$.prototype,"routerSlot",2);_([re("ia-topnav")],$.prototype,"topNav",2);_([re("modal-manager")],$.prototype,"modalManager",2);_([re("#maincontent")],$.prototype,"mainContent",2);$=_([A("app-root")],$);class Ps{setup(){var a;(window._mtm=window._mtm||[]).push({"mtm.startTime":new Date().getTime(),event:"mtm.Start"});const t=window.location.hostname,r={"archive.org":"ZUUaFed2","web.archive.org":"kzRD6OIl",fallback:"x5vxauv0"},n=r[t]??r.fallback,s=document,o=s.createElement("script"),l=s.getElementsByTagName("script")[0];o.async=!0,o.src="//apollo.archive.org/js/container_"+n+".js",(a=l.parentNode)==null||a.insertBefore(o,l)}}const Ms=new Ps;Ms.setup();export{y as A,qs as B,H as C,Or as D,as as E,fs as F,ys as G,si as H,Gi as I,bs as J,Ks as K,rr as L,ct as P,we as T,f as _,re as a,w as b,Ct as c,As as d,A as e,ps as f,ms as g,k as h,g as i,mo as j,Os as k,ne as l,M as m,h as n,Lt as o,mr as p,js as q,$s as r,P as s,S as t,c as u,Nr as v,Yi as w,p as x,Ys as y,Cr as z};
